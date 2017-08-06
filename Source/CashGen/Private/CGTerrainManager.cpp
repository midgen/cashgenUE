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
			0, EThreadPriority::TPri_BelowNormal, FPlatformAffinity::GetNoAffinityMask()));
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
}

FIntVector2 ACGTerrainManager::GetSector(const FVector& aLocation)
{
	FIntVector2 sector;
	sector.X = static_cast<int32>(aLocation.X) / 1000;
	sector.Y = static_cast<int32>(aLocation.Y) / 1000;

	return sector;
}


TArray<FIntVector2> ACGTerrainManager::GetRelevantSectorsForActor(const AActor* anActor)
{
	TArray<FIntVector2> result;

	FIntVector2 rootSector = GetSector(anActor->GetActorLocation());


	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			result.Emplace(rootSector.X - 5 + x, rootSector.Y - 5 + y);
		}
	}

	return result;
}

void ACGTerrainManager::SetTerrainConfig(FCGTerrainConfig aTerrainConfig)
{

}

void ACGTerrainManager::HandlePlayerSectorChange(const uint8 aPlayerID, const FIntVector2& anOldSector, const FIntVector2& aNewSector)
{

}

void ACGTerrainManager::AddPawn()
{

}


void ACGTerrainManager::CreateTileRefreshJob(FCGJob aJob)
{
	if (aJob.LOD != 10)
	{
		myPendingJobQueue.Enqueue(aJob);
		QueuedTiles.Add(aJob.Tile);
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