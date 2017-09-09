#include "CashGen.h"
#include "CGTerrainManager.h"
#include "CGJob.h"

ACGTerrainManager::ACGTerrainManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

ACGTerrainManager::~ACGTerrainManager()
{

}


void ACGTerrainManager::BeginPlay()
{
	Super::BeginPlay();

	FString threadName = "TerrainWorkerThread";

	for (int i = 0; i < myTerrainConfig.NumberOfThreads; i++)
	{
		myGeometryJobQueues.Emplace();

		myWorkerThreads.Add(FRunnableThread::Create
		(new FCGTerrainGeneratorWorker(this, &myTerrainConfig, &myGeometryJobQueues[i]),
			*threadName,
			0, EThreadPriority::TPri_Normal, FPlatformAffinity::GetNoAffinityMask()));
	}
}

void ACGTerrainManager::BeginDestroy()
{
	for (auto& thread : myWorkerThreads)
	{
		if (thread != nullptr)
		{
			thread->Kill();
		}
	}

	Super::BeginDestroy();
}

void ACGTerrainManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	myTimeSinceLastSweep += DeltaSeconds;


	// Check for pending jobs
	for (int i = 0; i < myTerrainConfig.NumberOfThreads; i++)
	{
		FCGJob pendingJob;
		if (myPendingJobQueue.Peek(pendingJob))
		{
			// If there's free data to allocate, dequeue and send to worker thread
			if (myFreeMeshData[pendingJob.LOD].Num() > 0)
			{
				myPendingJobQueue.Dequeue(pendingJob);
				GetFreeMeshData(pendingJob);
				myGeometryJobQueues[i].Enqueue(pendingJob);
			}
		}
	}

	// Now check for Update jobs
	for (uint8 i = 0; i < myTerrainConfig.MeshUpdatesPerFrame; i++)
	{
		FCGJob updateJob;
		if (myUpdateJobQueue.Dequeue(updateJob))
		{
			milliseconds startMs = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				);

			updateJob.myTileHandle.myHandle->UpdateMesh(updateJob.LOD, updateJob.IsInPlaceUpdate, &updateJob.Data->MyVertexData, &updateJob.Data->MyTriangles );

			updateJob.myTileHandle.myHandle->SetActorHiddenInGame(false);
			int32 updateMS = (duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				) - startMs).count();

#ifdef UE_BUILD_DEBUG
			if (updateJob.LOD == 0)
			{
				GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Heightmap gen " + FString::FromInt(updateJob.HeightmapGenerationDuration) + "ms"));
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Erosion gen " + FString::FromInt(updateJob.ErosionGenerationDuration) + "ms"));
				GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, TEXT("MeshUpdate " + FString::FromInt(updateMS) + "ms"));
			}
#endif
			ReleaseMeshData(updateJob.LOD, updateJob.Data);
			myQueuedSectors.Remove(updateJob.mySector);
		}
	}

	// Time based sweep of actors to see if any have moved sectors
	if (myTimeSinceLastSweep > myTerrainConfig.TileSweepTime && myTrackedActors.Num() > 0)
	{
			// Compare current location to previous
			FIntVector2 oldSector = myActorLocationMap[myTrackedActors[myActorIndex]];
			FIntVector2 newSector = GetSector(myTrackedActors[myActorIndex]->GetActorLocation());
			if (oldSector != newSector)
			{
				// Take care of spawning new sectors if necessary
				HandlePlayerSectorChange(myTrackedActors[myActorIndex], oldSector, newSector);
				
				ProcessTilesForActor(myTrackedActors[myActorIndex]);
			}
			else
			{
				// TODO: This is crap too, must be a better way.....
				for (FCGSector& sector : GetRelevantSectorsForActor(myTrackedActors[myActorIndex]))
				{
					if (myTileHandleMap.Contains(sector.mySector))
					{
						myTileHandleMap[sector.mySector].myLastRequiredTimestamp = FDateTime::Now();
					}
				}
			}

			if (myActorIndex < myTrackedActors.Num() - 1)
			{
				myActorIndex++;
			}
			else
			{ 
				myActorIndex = 0;
				myTimeSinceLastSweep = 0.0f;
			}

			// TODO: this sucks, don't wanna be iterating over a big map like this
			for (auto& elem : myTileHandleMap)
			{
				// The tile hasn't been required  free it
				if (elem.Value.myLastRequiredTimestamp + myTerrainConfig.TileReleaseDelay < FDateTime::Now())
				{
					FreeTile(elem.Value.myHandle);
					myTileHandleMap.Remove(elem.Key);
				}
			}
	}

	if (!myIsTerrainComplete &&
			myTrackedActors.Num() > 0 &&
		    myPendingJobQueue.IsEmpty() &&
			myUpdateJobQueue.IsEmpty())
	{
		BroadcastTerrainComplete();
		myIsTerrainComplete = true;
	}
}

ACGTile* ACGTerrainManager::GetFreeTile()
{
	ACGTile* result = nullptr;

	if (myFreeTiles.Num())
	{
		result = myFreeTiles.Pop();
	}
	
	
	if (!result)
	{
		result = GetWorld()->SpawnActor<ACGTile>(ACGTile::StaticClass(), FVector(0.0f, 0.0f, -10000.0f), FRotator(0.0f));
	}

	return result;
}

void ACGTerrainManager::FreeTile(ACGTile* aTile)
{
	aTile->SetActorHiddenInGame(true);
	myFreeTiles.Push(aTile);
}

void ACGTerrainManager::HandlePlayerSectorChange(const AActor* aActor, const FIntVector2& anOldSector, const FIntVector2& aNewSector)
{
	myActorLocationMap[aActor] = aNewSector;
}

FIntVector2 ACGTerrainManager::GetSector(const FVector& aLocation)
{
	FIntVector2 sector;;

	sector.X = FMath::RoundToInt(aLocation.X / (myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize));
	sector.Y = FMath::RoundToInt(aLocation.Y / (myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize));

	return sector;
}


TArray<FCGSector> ACGTerrainManager::GetRelevantSectorsForActor(const AActor* aActor)
{
	TArray<FCGSector> result;

	FIntVector2 rootSector = GetSector(aActor->GetActorLocation());
	
	// Always include the sector the pawn is in
	result.Add(rootSector);

	const int sweepRange = myTerrainConfig.LODs[myTerrainConfig.LODs.Num() - 1].SectorDistance;
	const int sweepRange2 = sweepRange * sweepRange;


	for (int x = 0; x < sweepRange * 2; x++)
	{
		for (int y = 0; y < sweepRange * 2; y++)
		{
			FCGSector newSector = FCGSector(rootSector.X - sweepRange + x, rootSector.Y - sweepRange + y, 0);
			FIntVector2 diff = newSector.mySector - rootSector;
			int thisRange = (diff.X * diff.X + diff.Y * diff.Y);
			int lod = GetLODForRange(thisRange);
			if (newSector != rootSector && lod > -1)
			{
				newSector.myLOD = lod;
				result.Add(newSector);
			}

		}
	}

	return result;
}

int ACGTerrainManager::GetLODForRange(const int32 aRange)
{
	int lowestLOD = 999;
	for (int i = myTerrainConfig.LODs.Num() - 1; i >= 0; i--)
	{
		if (aRange < (myTerrainConfig.LODs[i].SectorDistance * myTerrainConfig.LODs[i].SectorDistance) && lowestLOD > i)
		{
			lowestLOD = i;
		}
	}

	return lowestLOD != 999 ? lowestLOD : -1;
}


void ACGTerrainManager::SetupTerrainGenerator(FCGTerrainConfig aTerrainConfig)
{
	myTerrainConfig = aTerrainConfig;

	myTerrainConfig.TileOffset = FVector(myTerrainConfig.UnitSize * myTerrainConfig.TileXUnits * 0.5f, myTerrainConfig.UnitSize * myTerrainConfig.TileYUnits * 0.5f, 0.0f);

	AllocateAllMeshDataStructures();

	isReady = true;
}

void ACGTerrainManager::AddActorToTrack(AActor* aPawn)
{
	myTrackedActors.Add(aPawn);
	FIntVector2 pawnSector = GetSector(aPawn->GetActorLocation());
	myActorLocationMap.Add(aPawn, pawnSector);

	ProcessTilesForActor(aPawn);

}



void ACGTerrainManager::CreateTileRefreshJob(FCGJob aJob)
{
	if (aJob.LOD != 10)
	{
		myPendingJobQueue.Enqueue(aJob);
		myQueuedSectors.Add(aJob.mySector);
	}

}

void ACGTerrainManager::ProcessTilesForActor(const AActor* anActor)
{
	for (FCGSector& sector : GetRelevantSectorsForActor(anActor))
	{
		bool isExistsAtLowerLOD = myTileHandleMap.Contains(sector.mySector) && myTileHandleMap[sector.mySector].myLOD > sector.myLOD;
		// If the sector doesn't have a tile already, or the tile that does exist is a higher LOD
		if (!myTileHandleMap.Contains(sector.mySector) || isExistsAtLowerLOD)
		{

			FCGTileHandle tileHandle;
			// We have to create the tile for this sector
			if (!isExistsAtLowerLOD)
			{
				FActorSpawnParameters spawnParameters;
				spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				tileHandle.myHandle = GetWorld()->SpawnActor<ACGTile>(ACGTile::StaticClass(),
					FVector((myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize * sector.mySector.X),
					(myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize * sector.mySector.Y), 0.0f), FRotator(0.0f), spawnParameters);

				tileHandle.myStatus = ETileStatus::SPAWNED;
				tileHandle.myLOD = sector.myLOD;
				tileHandle.myLastRequiredTimestamp = FDateTime::Now();

				// Add it to our sector map
				myTileHandleMap.Add(sector.mySector, tileHandle);
			}
			else
			{
				myTileHandleMap[sector.mySector].myLOD = sector.myLOD;
				tileHandle = myTileHandleMap[sector.mySector];
			}

			// Create the job to generate the new geometry and update the terrain tile
			FCGJob job;
			job.mySector = sector.mySector;
			job.myTileHandle = tileHandle;
			job.LOD = sector.myLOD;
			job.IsInPlaceUpdate = isExistsAtLowerLOD;

			// TODO: this method needs renaming
			tileHandle.myHandle->UpdateSettings(sector.mySector, &myTerrainConfig, FVector(0.f));

			if(!isExistsAtLowerLOD) {
				tileHandle.myHandle->RepositionAndHide(10);
			}

			CreateTileRefreshJob(job);
		}
	}
}

bool ACGTerrainManager::GetFreeMeshData(FCGJob& aJob)
{
	// No free mesh data
	if (myFreeMeshData[aJob.LOD].Num() < 1)
	{
		return false;
	}
	else
	{
		FCGMeshData* dataToUse;
		// Use the first free data set, there'll always be one, we checked!
		for (FCGMeshData* data : myFreeMeshData[aJob.LOD])
		{
			dataToUse = data;
			break;
		}
		// Add to the in use set
		myInUseMeshData[aJob.LOD].Add(dataToUse);
		// Remove from the Free set
		myFreeMeshData[aJob.LOD].Remove(dataToUse);

		aJob.Data = dataToUse;
		return true;
	}

	return false;
}

void ACGTerrainManager::ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease)
{
	myInUseMeshData[aLOD].Remove(aDataToRelease);
	myFreeMeshData[aLOD].Add(aDataToRelease);
}

/** Allocates data structures and pointers for mesh data **/
void ACGTerrainManager::AllocateAllMeshDataStructures()
{
	for (uint8 lod = 0; lod < myTerrainConfig.LODs.Num(); ++lod)
	{
		myMeshData.Add(FCGLODMeshData());
		myFreeMeshData.Add(TSet<FCGMeshData*>());
		myInUseMeshData.Add(TSet<FCGMeshData*>());

		myMeshData[lod].Data.Reserve(myTerrainConfig.MeshDataPoolSize);

		for (int j = 0; j < myTerrainConfig.MeshDataPoolSize; ++j)
		{
			myMeshData[lod].Data.Add(FCGMeshData());
			AllocateDataStructuresForLOD(&myMeshData[lod].Data[j], &myTerrainConfig, lod);
		}
	}

	for (uint8 lod = 0; lod < myTerrainConfig.LODs.Num(); ++lod)
	{
		for (int j = 0; j < myTerrainConfig.MeshDataPoolSize; ++j)
		{
			myFreeMeshData[lod].Add(&myMeshData[lod].Data[j]);
		}
	}

}

/************************************************************************/
/*  Allocates all the data structures for a single LOD mesh data
/*		Includes setting up triangles etc.
/************************************************************************/
bool ACGTerrainManager::AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD)
{
	int32 numXVerts = aLOD == 0 ? aConfig->TileXUnits + 1 : (aConfig->TileXUnits / myTerrainConfig.LODs[aLOD].ResolutionDivisor) + 1;
	int32 numYVerts = aLOD == 0 ? aConfig->TileYUnits + 1 : (aConfig->TileYUnits / myTerrainConfig.LODs[aLOD].ResolutionDivisor) + 1;

	int32 numTotalVertices = numXVerts * numYVerts + (aConfig->TileXUnits * 2) + (aConfig->TileYUnits * 2) + 4;

	aData->MyVertexData.Reserve(numTotalVertices);

	// Generate the per vertex data sets
	for (int32 i = 0; i < (numTotalVertices); ++i)
	{
		aData->MyVertexData.Emplace();
	}

	// Heightmap needs to be larger than the mesh
	// Using vectors here is a lot wasteful, but it does make normal/tangent or any other
	// Geometric calculations based on the heightmap a bit easier. Easy enough to change to floats

	aData->HeightMap.Reserve((numXVerts + 2) * (numYVerts + 2));
	for (int32 i = 0; i < (numXVerts + 2) * (numYVerts + 2); ++i)
	{
		aData->HeightMap.Emplace(0.0f);
	}

	// Triangle indexes
	int32 terrainTris = ((numXVerts - 1) * (numYVerts - 1) * 6);
	int32 skirtTris = (((numXVerts - 1) * 2) + ((numYVerts - 1) * 2)) * 6;
	int32 numTris = terrainTris + skirtTris;
	aData->MyTriangles.Reserve(numTris);
	for (int32 i = 0; i < numTris; ++i)
	{
		aData->MyTriangles.Add(i);
	}

	// Now calculate triangles and UVs
	int32 triCounter = 0;
	int32 thisX, thisY;
	int32 rowLength;

	rowLength = aLOD == 0 ? aConfig->TileXUnits + 1 : (aConfig->TileXUnits / myTerrainConfig.LODs[aLOD].ResolutionDivisor + 1);
	float maxUV = aLOD == 0 ? 1.0f : 1.0f / aLOD;

	int32 exX = aLOD == 0 ? aConfig->TileXUnits : (aConfig->TileXUnits / myTerrainConfig.LODs[aLOD].ResolutionDivisor);
	int32 exY = aLOD == 0 ? aConfig->TileYUnits : (aConfig->TileYUnits / myTerrainConfig.LODs[aLOD].ResolutionDivisor);

	for (int32 y = 0; y < exY; ++y)
	{
		for (int32 x = 0; x < exX; ++x)
		{

			thisX = x;
			thisY = y;
			//TR
			aData->MyTriangles[triCounter] = thisX + ((thisY + 1) * (rowLength));
			triCounter++;
			//BL
			aData->MyTriangles[triCounter] = (thisX + 1) + (thisY * (rowLength));
			triCounter++;
			//BR
			aData->MyTriangles[triCounter] = thisX + (thisY * (rowLength));
			triCounter++;

			//BL
			aData->MyTriangles[triCounter] = (thisX + 1) + (thisY * (rowLength));
			triCounter++;
			//TR
			aData->MyTriangles[triCounter] = thisX + ((thisY + 1) * (rowLength));
			triCounter++;
			// TL
			aData->MyTriangles[triCounter] = (thisX + 1) + ((thisY + 1) * (rowLength));
			triCounter++;

			//TR
			aData->MyVertexData[thisX + ((thisY + 1) * (rowLength))].UV0 = FVector2D(thisX * maxUV, (thisY + 1.0f) * maxUV);
			//BR
			aData->MyVertexData[thisX + (thisY * (rowLength))].UV0 = FVector2D(thisX * maxUV, thisY * maxUV);
			//BL
			aData->MyVertexData[(thisX + 1) + (thisY * (rowLength))].UV0 = FVector2D((thisX + 1.0f) * maxUV, thisY * maxUV);
			//TL
			aData->MyVertexData[(thisX + 1) + ((thisY + 1) * (rowLength))].UV0 = FVector2D((thisX + 1.0f)* maxUV, (thisY + 1.0f) * maxUV);

		}
	}

	return true;

}

