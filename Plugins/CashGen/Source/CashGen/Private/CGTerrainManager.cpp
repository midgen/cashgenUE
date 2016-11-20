#include "cashgen.h"
#include "CGTerrainManager.h"


ACGTerrainManager::ACGTerrainManager()
{
	PrimaryActorTick.bCanEverTick = true;



}

void ACGTerrainManager::BeginPlay()
{
	Super::BeginPlay();

	// Add array entries for each LOD
	MeshData.Add(TArray<FCGMeshData>());
	MeshData.Add(TArray<FCGMeshData>());
	MeshData.Add(TArray<FCGMeshData>());

	FreeMeshData.Add(TSet<FCGMeshData*>());
	FreeMeshData.Add(TSet<FCGMeshData*>());
	FreeMeshData.Add(TSet<FCGMeshData*>());

	InUseMeshData.Add(TSet<FCGMeshData*>());
	InUseMeshData.Add(TSet<FCGMeshData*>());
	InUseMeshData.Add(TSet<FCGMeshData*>());

	FString threadName = "TerrainWorkerThread";

	WorkerThread = FRunnableThread::Create
		(new FCGTerrainGeneratorWorker(this, &TerrainConfig),
			*threadName,
			0, EThreadPriority::TPri_BelowNormal, FPlatformAffinity::GetNoAffinityMask());
}

void ACGTerrainManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TimeSinceLastSweep += DeltaSeconds;

	if (isSetup)
	{
		// First check for the tracking actor crossing a tile boundary
		CGPoint oldPos = currentPlayerZone;

		if (TrackingActor->IsValidLowLevel())
		{
			currentPlayerZone.X = floor(TrackingActor->GetActorLocation().X / (TerrainConfig.UnitSize * TerrainConfig.XUnits));
			currentPlayerZone.Y = floor(TrackingActor->GetActorLocation().Y / (TerrainConfig.UnitSize * TerrainConfig.YUnits));
		}

		CGPoint newPos = currentPlayerZone;

		if (oldPos != newPos)
		{
			HandleTileFlip(newPos - oldPos);
		}

		// Now check for Update jobs
		FCGJob updateJob;
		if (UpdateJobs.Dequeue(updateJob))
		{
			updateJob.Tile->UpdateMesh(updateJob.LOD, updateJob.Data);
		}

		// Now check for LOD sweeps;
		if (TimeSinceLastSweep > SweepInterval)
		{
			SweepLODs();
			TimeSinceLastSweep = 0.0f;
		}
	}
}

void ACGTerrainManager::SweepLODs()
{
	for (ACGTile* tile : Tiles)
	{
		uint8 newLOD = GetLODForTile(tile);
		// LOD has decreased, we need to generate geometry
		if (tile->CurrentLOD > newLOD)
		{
			FCGJob newJob;
			newJob.Tile = tile;
			newJob.LOD = newLOD;
			CreateTileRefreshJob(newJob);
		}
	}
}


uint8 ACGTerrainManager::GetLODForTile(ACGTile* aTile)
{
	FVector diff;

	FVector centreOfTile = aTile->GetCentrePos();

	float distance = diff.Size();

	if (distance <= TerrainConfig.LOD1Range)
	{
		return 0;
	}
	if(distance > TerrainConfig.LOD1Range && distance <= TerrainConfig.LOD2Range)
	{
		return 1;
	}
	if (distance > TerrainConfig.LOD2Range && distance <= TerrainConfig.LODCullRange)
	{
		return 2;
	}
	// Otherwise it's in LOD cull range (don't render)
	return 10;
}

void ACGTerrainManager::CreateTileRefreshJob(FCGJob aJob)
{
	if (aJob.LOD != 10)
	{
		// Fetch a free data set
		aJob.Data = GetFreeMeshData(aJob.LOD);
		GeometryJobs.Enqueue(aJob);
	}

}


void ACGTerrainManager::HandleTileFlip(CGPoint deltaTile)
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
			tile->Offset.X;
		}
		if (tile->Offset.Y < minY) {
			minY = tile->Offset.Y;
		}
		if (tile->Offset.Y > maxY) {
			tile->Offset.Y;
		}
	}

	for (ACGTile* tile : Tiles)
	{
		if (deltaTile.X < 0.1 && tile->Offset.X == maxX) {
			tile->Offset.X = minX - 1;
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile);
			CreateTileRefreshJob(job);
		}
		if (deltaTile.X > 0.1 && tile->Offset.X == minX) {
			tile->Offset.X = maxX + 1;
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile);
			CreateTileRefreshJob(job);
		}
		if (deltaTile.Y < 0.1 && tile->Offset.Y == maxY) {
			tile->Offset.Y = minY - 1;
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile);
			CreateTileRefreshJob(job);
		}
		if (deltaTile.Y > 0.1 && tile->Offset.Y == minY) {
			tile->Offset.Y = maxY + 1;
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile);
			CreateTileRefreshJob(job);
		}
	}
}

FCGMeshData* ACGTerrainManager::GetFreeMeshData(uint8 aLOD)
{
	// No free mesh data
	if (FreeMeshData[aLOD].Num() < 1)
	{
		int32 newDataIndex = MeshData[aLOD].Emplace();
		InUseMeshData[aLOD].Add(&MeshData[aLOD][newDataIndex]);
		((FCGMeshData)MeshData[aLOD][newDataIndex]).AllocateDataStructuresForLOD(&TerrainConfig, aLOD);
		return &MeshData[aLOD][newDataIndex];
	}
	else
	{
		FCGMeshData* dataToUse;
		// Use the first free data set, there'll always be one, we checked!
		for (FCGMeshData* data : FreeMeshData[aLOD])
		{
			dataToUse = data;
			break;
		}
		// Add to the in use set
		InUseMeshData[aLOD].Add(dataToUse);
		// Remove from the Free set
		FreeMeshData[aLOD].Remove(dataToUse);
		return dataToUse;
	}

	return nullptr;
}

void ACGTerrainManager::ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease)
{
	InUseMeshData[aLOD].Remove(aDataToRelease);
	FreeMeshData[aLOD].Add(aDataToRelease);
}


void ACGTerrainManager::SpawnTiles(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles)
{
	TerrainConfig = aTerrainConfig;
	TrackingActor = aTrackingActor;
	XTiles = aXTiles;
	YTiles = aYTiles;

	WorldOffset = FVector((XTiles / 2.0f) * TerrainConfig.XUnits * TerrainConfig.UnitSize, (YTiles / 2.0f) * TerrainConfig.YUnits * TerrainConfig.UnitSize, 0.0f);
	
	if (aTrackingActor)
	{
		currentPlayerZone.X = floor(aTrackingActor->GetActorLocation().X / ((TerrainConfig.UnitSize * TerrainConfig.XUnits) - WorldOffset.X));
		currentPlayerZone.Y = floor(aTrackingActor->GetActorLocation().Y / ((TerrainConfig.UnitSize * TerrainConfig.YUnits) - WorldOffset.Y));
	}

	for (int32 i = 0; i < XTiles * YTiles; ++i)
	{
		Tiles.Add(GetWorld()->SpawnActor<ACGTile>(ACGTile::StaticClass(),
													FVector((TerrainConfig.XUnits * TerrainConfig.UnitSize * GetXYfromIdx(i).X) - WorldOffset.X,
																(TerrainConfig.YUnits * TerrainConfig.UnitSize * GetXYfromIdx(i).Y) - WorldOffset.Y, 0.0f) - WorldOffset, FRotator(0.0f)));
	}

	int32 tileIndex = 0;
	for (ACGTile* tile : Tiles)
	{
		tile->SetupTile(GetXYfromIdx(tileIndex), &TerrainConfig, WorldOffset);
		FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
		CreateTileRefreshJob(job);
		++tileIndex;
	}

}

