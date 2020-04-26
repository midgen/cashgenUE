
#include "CashGen/Public/CGTerrainManager.h"
#include "CashGen/Public/CGTerrainGeneratorWorker.h"
#include "CashGen/Public/CGTile.h"
#include "CashGen/Public/Struct/CGJob.h"
#include "CashGen/Public/Struct/CGTileHandle.h"

#include <chrono>

using namespace std::chrono;

DECLARE_CYCLE_STAT(TEXT("CashGenStat ~ ActorSectorSweeps"), STAT_ActorSectorSweeps, STATGROUP_CashGenStat);
DECLARE_CYCLE_STAT(TEXT("CashGenStat ~ SectorExpirySweeps"), STAT_SectorExpirySweeps, STATGROUP_CashGenStat);

ACGTerrainManager::ACGTerrainManager()
{
	PrimaryActorTick.bCanEverTick = true;

	MyWaterMeshComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("MyWaterComponent"));
	MyWaterMeshComponent->SetupAttachment(RootComponent);
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
		myWorkerThreads.Add(FRunnableThread::Create(new FCGTerrainGeneratorWorker(*this, myTerrainConfig, myFreeMeshData),
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
			delete thread;
			thread = nullptr;
		}
	}

	Super::BeginDestroy();
}

void ACGTerrainManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	myTimeSinceLastSweep += DeltaSeconds;

	// Make sure there's no daft number of threads. Unlikely you'll want more than one anyway.
	if (myTerrainConfig.NumberOfThreads > FPlatformMisc::NumberOfCores())
	{
		myTerrainConfig.NumberOfThreads = FPlatformMisc::NumberOfCores();
	}

	// Now check for Update jobs
	for (uint8 i = 0; i < myTerrainConfig.MeshUpdatesPerFrame; i++)
	{
		FCGJob updateJob;
		if (myUpdateJobQueue.Dequeue(updateJob))
		{
			milliseconds startMs = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch());

			updateJob.myTileHandle.myHandle->UpdateMesh(updateJob.LOD,
				updateJob.IsInPlaceUpdate,
				updateJob.Data->MyPositions,
				updateJob.Data->MyNormals,
				updateJob.Data->MyTangents,
				updateJob.Data->MyUV0,
				updateJob.Data->MyColours,
				updateJob.Data->MyTriangles,
				updateJob.Data->myTextureData);

			if (myTerrainConfig.UseInstancedWaterMesh)
			{
				FTransform waterTransform = FTransform(FRotator(0.0f), updateJob.myTileHandle.myHandle->GetActorLocation() + FVector(myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize * 0.5f, myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize * 0.5f, 0.0f), FVector(myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize * 0.01f, myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize * 0.01f, 1.0f));
				MyWaterMeshComponent->UpdateInstanceTransform(updateJob.myTileHandle.myWaterISMIndex, waterTransform, true, true, true);
			}

			updateJob.myTileHandle.myHandle->SetActorHiddenInGame(false);
			int32 updateMS = (duration_cast<milliseconds>(
								  system_clock::now().time_since_epoch()) -
							  startMs)
								 .count();

#ifdef UE_BUILD_DEBUG
			if (Settings->ShowTimings && updateJob.LOD == 0)
			{
				GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Heightmap gen " + FString::FromInt(updateJob.HeightmapGenerationDuration) + "ms"));
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Erosion gen " + FString::FromInt(updateJob.ErosionGenerationDuration) + "ms"));
				GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, TEXT("MeshUpdate " + FString::FromInt(updateMS) + "ms"));
			}
#endif

			updateJob.Data.Release();
			OnAfterTileCreated(updateJob.myTileHandle.myHandle);
			myQueuedSectors.Remove(updateJob.mySector);
		}
	}

	if (myActorIndex >= myTrackedActors.Num())
	{
		myActorIndex = FMath::Min(0, myTrackedActors.Num() - 1);
	}

	// Time based sweep of actors to see if any have moved sectors
	if (myTimeSinceLastSweep > myTerrainConfig.TileSweepTime && myTrackedActors.Num() > 0)
	{
		SCOPE_CYCLE_COUNTER(STAT_ActorSectorSweeps);

		// Compare current location to previous
		FIntVector2 oldSector = myActorLocationMap[myTrackedActors[myActorIndex]];
		FIntVector2 newSector = GetSector(myTrackedActors[myActorIndex]->GetActorLocation());
		if (oldSector != newSector)
		{
			// Take care of spawning new sectors if necessary
			SetActorSector(myTrackedActors[myActorIndex], newSector);

			ProcessTilesForActor(myTrackedActors[myActorIndex]);
		}
		else
		{
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
	}

	// TODO: this sucks, don't wanna be iterating over a big map like this
	// But the cost is negligible compared to the mesh updates/collision cooking we're doing sooooooo
	// Better than all the tiles ticking themselves

	TArray<FIntVector2> TilesToDelete;

	{
		SCOPE_CYCLE_COUNTER(STAT_SectorExpirySweeps);

		for (auto& elem : myTileHandleMap)
		{
			// The tile hasn't been required  free it
			if (elem.Value.myLastRequiredTimestamp + myTerrainConfig.TileReleaseDelay < FDateTime::Now())
			{
				FreeTile(elem.Value.myHandle, elem.Value.myWaterISMIndex);
				TilesToDelete.Push(elem.Key);
			}
			else if (myTerrainConfig.DitheringLODTransitions)
			{
				elem.Value.myHandle->TickTransition(DeltaSeconds);
			}
		}

		for (auto& key : TilesToDelete)
		{
			myTileHandleMap.Remove(key);
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

TPair<ACGTile*, int32> ACGTerrainManager::GetAvailableTile()
{
	TPair<ACGTile*, int32> result;

	if (myFreeTiles.Num())
	{
		result.Key = myFreeTiles.Pop();
		if (myTerrainConfig.UseInstancedWaterMesh)
		{
			result.Value = myFreeWaterMeshIndices.Pop();
		}
	}

	if (!result.Key)
	{
		result.Key = GetWorld()->SpawnActor<ACGTile>(ACGTile::StaticClass(), FVector(0.0f, 0.0f, -10000.0f), FRotator(0.0f));
		if (myTerrainConfig.UseInstancedWaterMesh)
		{
			result.Value = MyWaterMeshComponent->AddInstance(FTransform(FRotator(0.0f), FVector(0.0f, 0.0f, -10000.0f), FVector::OneVector));
		}
	}

	return result;
}

void ACGTerrainManager::FreeTile(ACGTile* aTile, const int32& waterMeshIndex)
{
	if (myTerrainConfig.UseInstancedWaterMesh)
	{
		MyWaterMeshComponent->UpdateInstanceTransform(waterMeshIndex, FTransform(FRotator(0.0f), FVector(0.0f, 0.0f, -100000.0f), FVector(0.1f)), true, true, true);
		myFreeWaterMeshIndices.Push(waterMeshIndex);
	}
	aTile->SetActorHiddenInGame(true);
	myFreeTiles.Push(aTile);
}

void ACGTerrainManager::SetActorSector(const AActor* aActor, const FIntVector2& aNewSector)
{
	myActorLocationMap[aActor] = aNewSector;
}

FIntVector2 ACGTerrainManager::GetSector(const FVector& aLocation)
{
	FIntVector2 sector;
	;

	sector.X = FMath::RoundToInt(aLocation.X / (myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize));
	sector.Y = FMath::RoundToInt(aLocation.Y / (myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize));

	return sector;
}

TArray<FCGSector> ACGTerrainManager::GetRelevantSectorsForActor(const AActor* aActor)
{
	TArray<FCGSector> result;

	FIntVector2 rootSector = GetSector(aActor->GetActorLocation());

	if (myTerrainConfig.LODs.Num() < 1)
	{
		return result;
	}

	// Always include the sector the pawn is in
	result.Add(rootSector);

	const int sweepRange = myTerrainConfig.LODs[myTerrainConfig.LODs.Num() - 1].SectorRadius;
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
		if (aRange < (myTerrainConfig.LODs[i].SectorRadius * myTerrainConfig.LODs[i].SectorRadius) && lowestLOD > i)
		{
			lowestLOD = i;
		}
	}

	return lowestLOD != 999 ? lowestLOD : -1;
}

void ACGTerrainManager::SetupTerrainGenerator(UUFNNoiseGenerator* aHeightmapGenerator, UUFNNoiseGenerator* aBiomeGenerator)
{
	//	myTerrainConfig = aTerrainConfig;

	myTerrainConfig.NoiseGenerator = aHeightmapGenerator;
	myTerrainConfig.BiomeBlendGenerator = aBiomeGenerator;

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

void ACGTerrainManager::RemoveActorToTrack(AActor* aPawn)
{
	myTrackedActors.Remove(aPawn);

	myActorLocationMap.Remove(aPawn);
}

void ACGTerrainManager::CreateTileRefreshJob(FCGJob aJob)
{
	if (aJob.LOD != 10)
	{
		myQueuedSectors.Add(aJob.mySector);
		myPendingJobQueue.Enqueue(std::move(aJob));
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
				TPair<ACGTile*, int32> tile = GetAvailableTile();
				tileHandle.myHandle = tile.Key;
				//tileHandle.myHandle->SetActorLocation(FVector(myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize * sector.mySector.X, myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize * sector.mySector.Y, 0.0f));

				if (myTerrainConfig.UseInstancedWaterMesh)
				{
					FTransform waterTransform = FTransform(FRotator(0.0f), FVector(myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize * (sector.mySector.X - 0.5f), myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize * (sector.mySector.Y - 0.5f), 0.0f), FVector(myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize * 0.01f, myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize * 0.01f, 1.0f));
					MyWaterMeshComponent->UpdateInstanceTransform(tile.Value, waterTransform, true, true, true);
				}

				tileHandle.myWaterISMIndex = tile.Value;
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

			if (!isExistsAtLowerLOD)
			{
				tileHandle.myHandle->RepositionAndHide(10);
				if (myTerrainConfig.UseInstancedWaterMesh)
				{
					MyWaterMeshComponent->UpdateInstanceTransform(tileHandle.myWaterISMIndex, FTransform(FRotator(0.0f), tileHandle.myHandle->GetActorLocation(), FVector(myTerrainConfig.TileXUnits * myTerrainConfig.UnitSize * 0.01f, myTerrainConfig.TileYUnits * myTerrainConfig.UnitSize * 0.01f, 1.0f)), true, true, true);
				}
			}

			CreateTileRefreshJob(std::move(job));
		}
	}
}

/** Allocates data structures and pointers for mesh data **/
void ACGTerrainManager::AllocateAllMeshDataStructures()
{
	for (uint8 lod = 0; lod < myTerrainConfig.LODs.Num(); ++lod)
	{
		myMeshData.Add(FCGLODMeshData());
		myFreeMeshData.Emplace();

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

/************************************************************************
  Allocates all the data structures for a single LOD mesh data
		Includes setting up triangles etc.
************************************************************************/
bool ACGTerrainManager::AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD)
{
	int32 numXVerts = aLOD == 0 ? aConfig->TileXUnits + 1 : (aConfig->TileXUnits / myTerrainConfig.LODs[aLOD].ResolutionDivisor) + 1;
	int32 numYVerts = aLOD == 0 ? aConfig->TileYUnits + 1 : (aConfig->TileYUnits / myTerrainConfig.LODs[aLOD].ResolutionDivisor) + 1;

	int32 numTotalVertices = numXVerts * numYVerts + ((numXVerts - 1) * 2) + ((numXVerts - 1) * 2);

	aData->MyPositions.Reserve(numTotalVertices);
	aData->MyNormals.Reserve(numTotalVertices);
	aData->MyTangents.Reserve(numTotalVertices);
	aData->MyColours.Reserve(numTotalVertices);
	aData->MyUV0.Reserve(numTotalVertices);
	if (myTerrainConfig.GenerateSplatMap)
	{
		aData->myTextureData.Reserve(aConfig->TileXUnits * aConfig->TileYUnits);
	}

	// Generate the per vertex data sets
	aData->MyPositions.AddDefaulted(numTotalVertices);
	aData->MyNormals.AddDefaulted(numTotalVertices);
	aData->MyTangents.AddDefaulted(numTotalVertices);
	aData->MyColours.AddDefaulted(numTotalVertices);
	aData->MyUV0.AddDefaulted(numTotalVertices);

	if (myTerrainConfig.GenerateSplatMap)
	{
		for (int32 i = 0; i < (aConfig->TileXUnits * aConfig->TileYUnits); ++i)
		{
			aData->myTextureData.Emplace();
		}
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

			////TR
			//aData->MyVertexData[thisX + ((thisY + 1) * (rowLength))].UV0 = FVector2D((thisX / rowLength) * maxUV, ((thisY / rowLength) + 1.0f) * maxUV);
			////BR
			//aData->MyVertexData[thisX + (thisY * (rowLength))].UV0 = FVector2D((thisX / rowLength) * maxUV, (thisY / rowLength) * maxUV);
			////BL
			//aData->MyVertexData[(thisX + 1) + (thisY * (rowLength))].UV0 = FVector2D(((thisX / rowLength) + 1.0f) * maxUV, (thisY / rowLength) * maxUV);
			////TL
			//aData->MyVertexData[(thisX + 1) + ((thisY + 1) * (rowLength))].UV0 = FVector2D(((thisX / rowLength) + 1.0f)* maxUV, ((thisY / rowLength) + 1.0f) * maxUV);

			//TR
			aData->MyUV0[thisX + ((thisY + 1) * (rowLength))] = FVector2D(thisX * 1.0f / rowLength, (thisY + 1.0f) / rowLength);
			//BR
			aData->MyUV0[thisX + (thisY * (rowLength))] = FVector2D(thisX * 1.0f / rowLength, thisY * 1.0f / rowLength);
			//BL
			aData->MyUV0[(thisX + 1) + (thisY * (rowLength))] = FVector2D((thisX + 1.0f) / rowLength, thisY * 1.0f / rowLength);
			//TL
			aData->MyUV0[(thisX + 1) + ((thisY + 1) * (rowLength))] = FVector2D((thisX + 1.0f) / rowLength, (thisY + 1.0f) / rowLength);
		}
	}

	return true;
}
