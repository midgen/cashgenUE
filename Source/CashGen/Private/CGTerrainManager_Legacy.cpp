#include "cashgen.h"
#include "CGTerrainManager.h"
#include "CGJob.h"
#include "CGTerrainManager_Legacy.h"


ACGTerrainManager_Legacy::ACGTerrainManager_Legacy()
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	bReplicates = true;
}

ACGTerrainManager_Legacy::~ACGTerrainManager_Legacy()
{

}

void ACGTerrainManager_Legacy::SetupTerrainClient(UUFNNoiseGenerator* aNoiseGen, UUFNNoiseGenerator* aBiomeBlendGen, AActor* aTrackingActor)
{
	TerrainConfig.NoiseGenerator = aNoiseGen;
	TerrainConfig.BiomeBlendGenerator = aBiomeBlendGen;
	TrackingActor = aTrackingActor; 
}

void ACGTerrainManager_Legacy::BeginPlay()
{
	//Super::BeginPlay();

	//FString threadName = "TerrainWorkerThread";

	//for (int i = 0; i < TerrainConfig.NumberOfThreads; i++)
	//{
	//	GeometryJobs.Emplace();

	//	WorkerThreads.Add(FRunnableThread::Create
	//	(new FCGTerrainGeneratorWorker(this, &TerrainConfig, &GeometryJobs[i]),
	//		*threadName,
	//		0, EThreadPriority::TPri_BelowNormal, FPlatformAffinity::GetNoAffinityMask()));
	//}
}

void ACGTerrainManager_Legacy::Tick(float DeltaSeconds)
{
	

	TimeSinceLastSweep += DeltaSeconds;

	if (TrackingActor->IsValidLowLevel())
	{
		//// First check for the tracking actor crossing a tile boundary
		//CGPoint oldPos = currentPlayerZone;

		//if (TrackingActor->IsValidLowLevel())
		//{
		//	currentPlayerZone.X = floor(TrackingActor->GetActorLocation().X / (TerrainConfig.UnitSize * TerrainConfig.TileXUnits));
		//	currentPlayerZone.Y = floor(TrackingActor->GetActorLocation().Y / (TerrainConfig.UnitSize * TerrainConfig.TileYUnits));
		//}

		//CGPoint newPos = currentPlayerZone;

		//if (oldPos != newPos)
		//{
		//	CGPoint delta = newPos - oldPos;
		//	HandleTileFlip(delta.X, delta.Y);
		//}

		//// Check for pending jobs
		//for (int i = 0; i < TerrainConfig.NumberOfThreads; i++)
		//{
		//	FCGJob pendingJob;
		//	if (PendingJobs.Peek(pendingJob))
		//	{
		//		// If there's free data to allocate, dequeue and send to worker thread
		//		if (FreeMeshData[pendingJob.LOD].Num() > 0)
		//		{
		//			PendingJobs.Dequeue(pendingJob);
		//			GetFreeMeshData(pendingJob);
		//			GeometryJobs[i].Enqueue(pendingJob);
		//		}
		//	}
		//}

		// Now check for Update jobs
		for (uint8 i = 0; i < TerrainConfig.MeshUpdatesPerFrame; i++)
		{
			FCGJob updateJob;
			if (UpdateJobs.Dequeue(updateJob))
			{
				milliseconds startMs = duration_cast<milliseconds>(
					system_clock::now().time_since_epoch()
					);

				updateJob.Tile->UpdateMesh(updateJob.LOD, updateJob.IsInPlaceUpdate, updateJob.Vertices, updateJob.Triangles, updateJob.Normals, updateJob.UV0, updateJob.VertexColors, updateJob.Tangents);

				OnTileMeshUpdated.Broadcast(updateJob.Tile);

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
				QueuedTiles.Remove(updateJob.Tile);
			}
		}


		// Now check for LOD sweeps;
		if (TimeSinceLastSweep > SweepInterval)
		{
			SweepLODs();
			TimeSinceLastSweep = 0.0f;
		}
	}
	//else
	//{
	//	if (TerrainConfig.NoiseGenerator != nullptr && XTiles > 0 && YTiles > 0)
	//	{
	//		SpawnTiles(TrackingActor, TerrainConfig, XTiles, YTiles);
	//	}
	//}

	if (isFirstDraw && PendingJobs.IsEmpty() && UpdateJobs.IsEmpty())
	{
		OnInitialTileDrawComplete();
		isFirstDraw = false;
	}
}

void ACGTerrainManager_Legacy::SweepLODs()
{
	if (!TrackingActor)
	{
		return;
	}
	for (ACGTile* tile : Tiles)
	{
		uint8 newLOD = GetLODForTile(tile);
		// LOD has decreased, we need to generate geometry
		if (tile->GetCurrentLOD() > newLOD && !QueuedTiles.Contains(tile))
		{
			FCGJob newJob;
			newJob.Tile = tile;
			newJob.LOD = newLOD;
			newJob.IsInPlaceUpdate = true;
			CreateTileRefreshJob(newJob);
		}
	}
}

void ACGTerrainManager_Legacy::BeginDestroy()
{
	for (auto& thread : WorkerThreads)
	{
		if (thread != nullptr)
		{
			thread->Kill();
		}
	}

	Super::BeginDestroy();
}

uint8 ACGTerrainManager_Legacy::GetLODForTile(ACGTile* aTile)
{
	FVector diff;

	FVector centreOfTile = aTile->GetCentrePos();

	diff = TrackingActor->GetActorLocation() - centreOfTile;

	float distance = diff.Size();
	uint8 lodIndex = 0;

	for (FCGLODConfig& lod : TerrainConfig.LODs)
	{
		if (distance < lod.Range)
		{
			return lodIndex;
		}
		++lodIndex;
	}

	return 10;
}

void ACGTerrainManager_Legacy::CreateTileRefreshJob(FCGJob aJob)
{
	if (aJob.LOD != 10)
	{
		PendingJobs.Enqueue(aJob);
		QueuedTiles.Add(aJob.Tile);
	}

}

bool ACGTerrainManager_Legacy::ServerCallTileFlip_Validate(int32 sectorX, int32 sectorY)
{
	return true;
}
void ACGTerrainManager_Legacy::ServerCallTileFlip_Implementation(int32 sectorX, int32 sectorY)
{
	HandleTileFlip(sectorX, sectorY);
}

bool ACGTerrainManager_Legacy::HandleTileFlip_Validate(int32 sectorX, int32 sectorY)
{
	return true;
}
void ACGTerrainManager_Legacy::HandleTileFlip_Implementation(int32 sectorX, int32 sectorY)
{
	// this section is still horrible, oh well!
	int32 minX = 999999999;
	int32 maxX = -99999999;
	int32 minY = 999999999;
	int32 maxY = -99999999;

	for (ACGTile* tile : Tiles)
	{
		if (tile->Offset.X < minX) {
			minX = tile->Offset.X;
		}
		if (tile->Offset.X > maxX) {
			maxX = tile->Offset.X;
		}
		if (tile->Offset.Y < minY) {
			minY = tile->Offset.Y;
		}
		if (tile->Offset.Y > maxY) {
			maxY = tile->Offset.Y;
		}
	}

	for (ACGTile* tile : Tiles)
	{
		if (sectorX < -0.1 && tile->Offset.X == maxX) {
			tile->Offset.X = minX - 1;
			tile->RepositionAndHide(GetLODForTile(tile));
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
			CreateTileRefreshJob(job);
		}
		else if (sectorX > 0.1 && tile->Offset.X == minX) {
			tile->Offset.X = maxX + 1;
			tile->RepositionAndHide(GetLODForTile(tile));
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
			CreateTileRefreshJob(job);
		}
		if (sectorY < -0.1 && tile->Offset.Y == maxY) {
			tile->Offset.Y = minY - 1;
			tile->RepositionAndHide(GetLODForTile(tile));
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
			CreateTileRefreshJob(job);
		}
		else if (sectorY > 0.1 && tile->Offset.Y == minY) {
			tile->Offset.Y = maxY + 1;
			tile->RepositionAndHide(GetLODForTile(tile));
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
			CreateTileRefreshJob(job);
		}
	}
}

bool ACGTerrainManager_Legacy::GetFreeMeshData(FCGJob& aJob)
{
	// No free mesh data
	if (FreeMeshData[aJob.LOD].Num() < 1)
	{
		return false;
	}
	else
	{
		FCGMeshData* dataToUse;
		// Use the first free data set, there'll always be one, we checked!
		for (FCGMeshData* data : FreeMeshData[aJob.LOD])
		{
			dataToUse = data;
			break;
		}
		// Add to the in use set
		InUseMeshData[aJob.LOD].Add(dataToUse);
		// Remove from the Free set
		FreeMeshData[aJob.LOD].Remove(dataToUse);

		aJob.Vertices = &dataToUse->Vertices;
		aJob.Triangles = &dataToUse->Triangles;
		aJob.Normals = &dataToUse->Normals;
		aJob.UV0 = &dataToUse->UV0;
		aJob.VertexColors = &dataToUse->VertexColors;
		aJob.Tangents = &dataToUse->Tangents;
		aJob.HeightMap = &dataToUse->HeightMap;
		aJob.Data = dataToUse;
		return true;
	}

	return false;
}

void ACGTerrainManager_Legacy::ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease)
{
	InUseMeshData[aLOD].Remove(aDataToRelease);
	FreeMeshData[aLOD].Add(aDataToRelease);
}

/** Allocates data structures and pointers for mesh data **/
void ACGTerrainManager_Legacy::AllocateAllMeshDataStructures()
{
	for (uint8 lod = 0; lod < TerrainConfig.LODs.Num(); ++lod)
	{
		MeshData.Add(FCGLODMeshData());
		FreeMeshData.Add(TSet<FCGMeshData*>());
		InUseMeshData.Add(TSet<FCGMeshData*>());

		MeshData[lod].Data.Reserve(TerrainConfig.MeshDataPoolSize);

		for (int j = 0; j < TerrainConfig.MeshDataPoolSize; ++j)
		{
			MeshData[lod].Data.Add(FCGMeshData());
			AllocateDataStructuresForLOD(&MeshData[lod].Data[j], &TerrainConfig, lod);
		}
	}

	for (uint8 lod = 0; lod < TerrainConfig.LODs.Num(); ++lod)
	{
		for (int j = 0; j < TerrainConfig.MeshDataPoolSize; ++j)
		{
			FreeMeshData[lod].Add(&MeshData[lod].Data[j]);
		}
	}

}

bool ACGTerrainManager_Legacy::ServerCallSpawnTiles_Validate(int32 sectorX, int32 sectorY)
{
	return true;
}
void ACGTerrainManager_Legacy::ServerCallSpawnTiles_Implementation(int32 sectorX, int32 sectorY)
{
	HandleSpawnTiles(sectorX, sectorY);
}
bool ACGTerrainManager_Legacy::HandleSpawnTiles_Validate(const int32 aXTiles, const int32 aYTiles)
{
	return true;
}




/************************************************************************/
/*  Main setup method, pass in the config struct and other details
/*				Spawns the Tile actors into place and sets them up
/************************************************************************/
//void ACGTerrainManager::SpawnTiles_Implementation(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles)
void ACGTerrainManager_Legacy::HandleSpawnTiles_Implementation(const int32 aXTiles, const int32 aYTiles)
{
	//TerrainConfig = aTerrainConfig;
	//TrackingActor = aTrackingActor;
	XTiles = aXTiles;
	YTiles = aYTiles;
	//currentPlayerZone.X = 0; currentPlayerZone.Y = 0;

	AllocateAllMeshDataStructures();

	WorldOffset = FVector((XTiles / 2.0f) * TerrainConfig.TileXUnits * TerrainConfig.UnitSize, (YTiles / 2.0f) * TerrainConfig.TileYUnits * TerrainConfig.UnitSize, 0.0f);
	
	//if (aTrackingActor)
	//{
		//currentPlayerZone.X = floor(aTrackingActor->GetActorLocation().X / ((TerrainConfig.UnitSize * TerrainConfig.TileXUnits) - WorldOffset.X));
		//currentPlayerZone.Y = floor(aTrackingActor->GetActorLocation().Y / ((TerrainConfig.UnitSize * TerrainConfig.TileYUnits) - WorldOffset.Y));
	//}

	FActorSpawnParameters spawnParameters;
	
	for (int32 i = 0; i < XTiles * YTiles; ++i)
	{
		//Tiles.Add(GetWorld()->SpawnActor<ACGTile>(ACGTile::StaticClass(),
		//											FVector((TerrainConfig.TileXUnits * TerrainConfig.UnitSize * GetXYfromIdx(i).X) - WorldOffset.X,
		//														(TerrainConfig.TileYUnits * TerrainConfig.UnitSize * GetXYfromIdx(i).Y) - WorldOffset.Y, 0.0f) - WorldOffset, FRotator(0.0f)));

	}

	int32 tileIndex = 0;
	for (ACGTile* tile : Tiles)
	{
		// Initially spawn at LOD10 (don't render)
		//tile->SetupTile(GetXYfromIdx(tileIndex), &TerrainConfig, WorldOffset);
		tile->RepositionAndHide(10);
		//FCGJob job; job.Tile = tile; job.LOD = 10; job.IsInPlaceUpdate = false;
		//CreateTileRefreshJob(job);
		++tileIndex;
	}

	//isSetup = true;
}

//bool ACGTerrainManager::SpawnTiles_Validate(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles)
//{
//	return true;
//}

/************************************************************************/
/*  Allocates all the data structures for a single LOD mesh data
/*		Includes setting up triangles etc.
/************************************************************************/
bool ACGTerrainManager_Legacy::AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD)
{
	int32 numXVerts = aLOD == 0 ? aConfig->TileXUnits + 1 : (aConfig->TileXUnits / TerrainConfig.LODs[aLOD].ResolutionDivisor) + 1;
	int32 numYVerts = aLOD == 0 ? aConfig->TileYUnits + 1 : (aConfig->TileYUnits / TerrainConfig.LODs[aLOD].ResolutionDivisor) + 1;

	int32 numTotalVertices = numXVerts * numYVerts + (aConfig->TileXUnits * 2) + (aConfig->TileYUnits * 2) + 4;

	aData->Vertices.Reserve(numTotalVertices);
	aData->Normals.Reserve(numTotalVertices);
	aData->UV0.Reserve(numTotalVertices);
	aData->VertexColors.Reserve(numTotalVertices);
	aData->Tangents.Reserve(numTotalVertices);

	// Generate the per vertex data sets
	for (int32 i = 0; i < (numTotalVertices); ++i)
	{
		aData->Vertices.Emplace(0.0f);
		aData->Normals.Emplace(0.0f, 0.0f, 1.0f);
		aData->UV0.Emplace(0.0f, 0.0f);
		aData->VertexColors.Emplace(FColor::Black);
		aData->Tangents.Emplace(0.0f, 0.0f, 0.0f);
	}

	// Heightmap needs to be larger than the mesh
	// Using vectors here is a bit wasteful, but it does make normal/tangent or any other
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
	aData->Triangles.Reserve(numTris);
	for (int32 i = 0; i < numTris; ++i)
	{
		aData->Triangles.Add(i);
	}

	// Now calculate triangles and UVs
	int32 triCounter = 0;
	int32 thisX, thisY;
	int32 rowLength;

	rowLength = aLOD == 0 ? aConfig->TileXUnits + 1 : (aConfig->TileXUnits / TerrainConfig.LODs[aLOD].ResolutionDivisor + 1);
	float maxUV = aLOD == 0 ? 1.0f : 1.0f / aLOD;

	int32 exX = aLOD == 0 ? aConfig->TileXUnits : (aConfig->TileXUnits / TerrainConfig.LODs[aLOD].ResolutionDivisor);
	int32 exY = aLOD == 0 ? aConfig->TileYUnits : (aConfig->TileYUnits / TerrainConfig.LODs[aLOD].ResolutionDivisor);

	for (int32 y = 0; y < exY; ++y)
	{
		for (int32 x = 0; x < exX; ++x)
		{

			thisX = x;
			thisY = y;
			//TR
			aData->Triangles[triCounter] = thisX + ((thisY + 1) * (rowLength));
			triCounter++;
			//BL
			aData->Triangles[triCounter] = (thisX + 1) + (thisY * (rowLength));
			triCounter++;
			//BR
			aData->Triangles[triCounter] = thisX + (thisY * (rowLength));
			triCounter++;

			//BL
			aData->Triangles[triCounter] = (thisX + 1) + (thisY * (rowLength));
			triCounter++;
			//TR
			aData->Triangles[triCounter] = thisX + ((thisY + 1) * (rowLength));
			triCounter++;
			// TL
			aData->Triangles[triCounter] = (thisX + 1) + ((thisY + 1) * (rowLength));
			triCounter++;

			//TR
			aData->UV0[thisX + ((thisY + 1) * (rowLength))] = FVector2D(thisX * maxUV, (thisY + 1.0f) * maxUV);
			//BR
			aData->UV0[thisX + (thisY * (rowLength))] = FVector2D(thisX * maxUV, thisY * maxUV);
			//BL
			aData->UV0[(thisX + 1) + (thisY * (rowLength))] = FVector2D((thisX + 1.0f) * maxUV, thisY * maxUV);
			//TL
			aData->UV0[(thisX + 1) + ((thisY + 1) * (rowLength))] = FVector2D((thisX + 1.0f)* maxUV, (thisY + 1.0f) * maxUV);

		}
	}

	return true;

}