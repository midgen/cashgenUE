#include "cashgen.h"
#include "CGTerrainManager.h"


ACGTerrainManager::ACGTerrainManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

ACGTerrainManager::~ACGTerrainManager()
{
	if(WorkerThread != nullptr)
	{
		WorkerThread->Kill(false);
	}
}

void ACGTerrainManager::BeginPlay()
{
	Super::BeginPlay();

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
			currentPlayerZone.X = floor(TrackingActor->GetActorLocation().X / (TerrainConfig.UnitSize * TerrainConfig.TileXUnits));
			currentPlayerZone.Y = floor(TrackingActor->GetActorLocation().Y / (TerrainConfig.UnitSize * TerrainConfig.TileYUnits));
		}

		CGPoint newPos = currentPlayerZone;

		if (oldPos != newPos)
		{
			CGPoint delta = newPos - oldPos;
			HandleTileFlip(delta);
			if (FMath::Abs(delta.X) > 0 && FMath::Abs(delta.Y))
			{
				GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Flipping " + FString::FromInt((newPos - oldPos).X) + " : " + FString::FromInt((newPos - oldPos).Y)));
			}
			
		}

		// Check for pending jobs
		FCGJob pendingJob;
		if (PendingJobs.Peek(pendingJob))
		{
			// If there's free data to allocate, dequeue and send to worker thread
			if (FreeMeshData[pendingJob.LOD].Num() > 0)
			{
				PendingJobs.Dequeue(pendingJob);
				GetFreeMeshData(pendingJob);
				GeometryJobs.Enqueue(pendingJob);
			}
		}

		// Now check for Update jobs
		FCGJob updateJob;
		if (UpdateJobs.Dequeue(updateJob))
		{
			updateJob.Tile->UpdateMesh(updateJob.LOD, updateJob.IsInPlaceUpdate, updateJob.Vertices, updateJob.Triangles, updateJob.Normals, updateJob.UV0, updateJob.VertexColors, updateJob.Tangents);
			ReleaseMeshData(updateJob.LOD, updateJob.Data);
			QueuedTiles.Remove(updateJob.Tile);
		}

		// Now check for LOD sweeps;
		if (TimeSinceLastSweep > SweepInterval)
		{
			SweepLODs();
			TimeSinceLastSweep = 0.0f;
		}
	}
	else
	{
		if (TerrainConfig.NoiseGenerator != nullptr)
		{
			SpawnTiles(TrackingActor, TerrainConfig, XTiles, YTiles);
		}
	}
}

void ACGTerrainManager::SweepLODs()
{
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


uint8 ACGTerrainManager::GetLODForTile(ACGTile* aTile)
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

void ACGTerrainManager::CreateTileRefreshJob(FCGJob aJob)
{
	if (aJob.LOD != 10)
	{
		PendingJobs.Enqueue(aJob);
		QueuedTiles.Add(aJob.Tile);
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
		if (deltaTile.X < -0.1 && tile->Offset.X == maxX) {
			tile->Offset.X = minX - 1;
			tile->RepositionAndHide(GetLODForTile(tile));
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
			CreateTileRefreshJob(job);
		}
		else if (deltaTile.X > 0.1 && tile->Offset.X == minX) {
			tile->Offset.X = maxX + 1;
			tile->RepositionAndHide(GetLODForTile(tile));
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
			CreateTileRefreshJob(job);
		}
		if (deltaTile.Y < -0.1 && tile->Offset.Y == maxY) {
			tile->Offset.Y = minY - 1;
			tile->RepositionAndHide(GetLODForTile(tile));
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
			CreateTileRefreshJob(job);
		}
		else if (deltaTile.Y > 0.1 && tile->Offset.Y == minY) {
			tile->Offset.Y = maxY + 1;
			tile->RepositionAndHide(GetLODForTile(tile));
			FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
			CreateTileRefreshJob(job);
		}
	}
}

bool ACGTerrainManager::GetFreeMeshData(FCGJob& aJob)
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
		aJob.DespositionMap = &dataToUse->DepositionMap;
		aJob.Data = dataToUse;
		return true;
	}

	return false;
}

void ACGTerrainManager::ReleaseMeshData(uint8 aLOD, FCGMeshData* aDataToRelease)
{
	InUseMeshData[aLOD].Remove(aDataToRelease);
	FreeMeshData[aLOD].Add(aDataToRelease);
}

/** Allocates data structures and pointers for mesh data **/
void ACGTerrainManager::AllocateAllMeshDataStructures()
{
	for (uint8 lod = 0; lod < TerrainConfig.LODs.Num(); ++lod)
	{
		MeshData.Add(FCGLODMeshData());
		FreeMeshData.Add(TSet<FCGMeshData*>());
		InUseMeshData.Add(TSet<FCGMeshData*>());

		MeshData[lod].Data.Reserve(MESH_DATA_POOL_SIZE);

		for (int j = 0; j < MESH_DATA_POOL_SIZE; ++j)
		{
			MeshData[lod].Data.Add(FCGMeshData());
			AllocateDataStructuresForLOD(&MeshData[lod].Data[j], &TerrainConfig, lod);
		}
	}

	for (uint8 lod = 0; lod < TerrainConfig.LODs.Num(); ++lod)
	{
		for (int j = 0; j < MESH_DATA_POOL_SIZE; ++j)
		{
			FreeMeshData[lod].Add(&MeshData[lod].Data[j]);
		}
	}

}

void ACGTerrainManager::SpawnTiles(AActor* aTrackingActor, const FCGTerrainConfig aTerrainConfig, const int32 aXTiles, const int32 aYTiles)
{
	TerrainConfig = aTerrainConfig;
	TrackingActor = aTrackingActor;
	XTiles = aXTiles;
	YTiles = aYTiles;
	currentPlayerZone.X = 0; currentPlayerZone.Y = 0;

	AllocateAllMeshDataStructures();

	WorldOffset = FVector((XTiles / 2.0f) * TerrainConfig.TileXUnits * TerrainConfig.UnitSize, (YTiles / 2.0f) * TerrainConfig.TileYUnits * TerrainConfig.UnitSize, 0.0f);
	
	if (aTrackingActor)
	{
		currentPlayerZone.X = floor(aTrackingActor->GetActorLocation().X / ((TerrainConfig.UnitSize * TerrainConfig.TileXUnits) - WorldOffset.X));
		currentPlayerZone.Y = floor(aTrackingActor->GetActorLocation().Y / ((TerrainConfig.UnitSize * TerrainConfig.TileYUnits) - WorldOffset.Y));
	}

	for (int32 i = 0; i < XTiles * YTiles; ++i)
	{
		Tiles.Add(GetWorld()->SpawnActor<ACGTile>(ACGTile::StaticClass(),
													FVector((TerrainConfig.TileXUnits * TerrainConfig.UnitSize * GetXYfromIdx(i).X) - WorldOffset.X,
																(TerrainConfig.TileYUnits * TerrainConfig.UnitSize * GetXYfromIdx(i).Y) - WorldOffset.Y, 0.0f) - WorldOffset, FRotator(0.0f)));

		// Set the correct LOD to prevent a loop on the initial spawn and generate cycle
		
	}

	int32 tileIndex = 0;
	for (ACGTile* tile : Tiles)
	{
		tile->SetupTile(GetXYfromIdx(tileIndex), &TerrainConfig, WorldOffset);
		tile->RepositionAndHide(GetLODForTile(tile));
		FCGJob job; job.Tile = tile; job.LOD = GetLODForTile(tile); job.IsInPlaceUpdate = false;
		CreateTileRefreshJob(job);
		++tileIndex;
	}
	isSetup = true;

}


bool ACGTerrainManager::AllocateDataStructuresForLOD(FCGMeshData* aData, FCGTerrainConfig* aConfig, const uint8 aLOD)
{
	int32 numXVerts = aLOD == 0 ? aConfig->TileXUnits + 1 : (aConfig->TileXUnits / TerrainConfig.LODs[aLOD].ResolutionDivisor) + 1;
	int32 numYVerts = aLOD == 0 ? aConfig->TileYUnits + 1 : (aConfig->TileYUnits / TerrainConfig.LODs[aLOD].ResolutionDivisor) + 1;

	aData->Vertices.Reserve(numXVerts * numYVerts);
	aData->Normals.Reserve(numXVerts * numYVerts);
	aData->UV0.Reserve(numXVerts * numYVerts);
	aData->VertexColors.Reserve(numXVerts * numYVerts);
	aData->Tangents.Reserve(numXVerts * numYVerts);

	// Generate the per vertex data sets
	for (int32 i = 0; i < (numXVerts * numYVerts); ++i)
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
	if (TerrainConfig.ThermalErosionIterations > 0) {
		aData->DepositionMap.Reserve((numXVerts + 2) * (numYVerts + 2));
	}
	for (int32 i = 0; i < (numXVerts + 2) * (numYVerts + 2); ++i)
	{
		aData->HeightMap.Emplace(0.0f);
		if (TerrainConfig.ThermalErosionIterations > 0) {
			aData->DepositionMap.Emplace(0.0f);
		}
	}

	// Triangle indexes
	aData->Triangles.Reserve((numXVerts - 1) * (numYVerts - 1) * 6);
	for (int32 i = 0; i < (numXVerts - 1) * (numYVerts - 1) * 6; ++i)
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