#include "CGTerrainGeneratorWorker.h"
#include "UFNNoiseGenerator.h"
#include "CGTile.h"
#include "RuntimeMeshGenericVertex.h"
#include <chrono>

using namespace std::chrono;

DECLARE_CYCLE_STAT(TEXT("CashGenStat ~ HeightMap"), STAT_HeightMap, STATGROUP_CashGenStat);
DECLARE_CYCLE_STAT(TEXT("CashGenStat ~ Normals"), STAT_Normals, STATGROUP_CashGenStat);
DECLARE_CYCLE_STAT(TEXT("CashGenStat ~ Erosion"), STAT_Erosion, STATGROUP_CashGenStat);


FCGTerrainGeneratorWorker::FCGTerrainGeneratorWorker(ACGTerrainManager* aTerrainManager, FCGTerrainConfig* aTerrainConfig, TQueue<FCGJob, EQueueMode::Spsc>* anInputQueue)
{
	pTerrainManager = aTerrainManager;
	pTerrainConfig = aTerrainConfig;
	inputQueue = anInputQueue;
}

FCGTerrainGeneratorWorker::~FCGTerrainGeneratorWorker()
{

}

bool FCGTerrainGeneratorWorker::Init()
{
	IsThreadFinished = false;
	return true;
}

uint32 FCGTerrainGeneratorWorker::Run()
{
	// Here's the loop
	while (!IsThreadFinished)
	{
		if (inputQueue->Dequeue(workJob))
		{
			pMeshData = workJob.Data;

			workLOD = workJob.LOD;

			milliseconds startMs = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				);

			prepMaps();
			ProcessTerrainMap();

			workJob.HeightmapGenerationDuration = (duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				) - startMs).count();

			startMs = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				);

			if (workLOD == 0)
			{
				{
					SCOPE_CYCLE_COUNTER(STAT_Erosion);

					for (int32 i = 0; i < pTerrainConfig->DropletAmount; ++i)
					{
						ProcessSingleDropletErosion();
					}
				}
			}

			workJob.ErosionGenerationDuration = (duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				) - startMs).count();

			ProcessPerBlockGeometry();
			ProcessPerVertexTasks();
			ProcessSkirtGeometry();


			pTerrainManager->myUpdateJobQueue.Enqueue(workJob);
		}
		// Otherwise, take a nap
		else
		{
			FPlatformProcess::Sleep(0.01f);
		}
	}

	return 1;
}

void FCGTerrainGeneratorWorker::Stop()
{
	IsThreadFinished = true;
}

void FCGTerrainGeneratorWorker::Exit()
{

}

void FCGTerrainGeneratorWorker::prepMaps()
{
	for (int32 i = 0; i < pMeshData->MyVertexData.Num(); ++i)
	{
		pMeshData->MyVertexData[i].Color.R = 0;
		pMeshData->MyVertexData[i].Color.G = 0;
		pMeshData->MyVertexData[i].Color.B = 0;
		pMeshData->MyVertexData[i].Color.A = 0;
	}
}

void FCGTerrainGeneratorWorker::ProcessTerrainMap()
{
	SCOPE_CYCLE_COUNTER(STAT_HeightMap);
	// Size of the noise sampling (larger than the actual mesh so we can have seamless normals)
	int32 exX = GetNumberOfNoiseSamplePoints();
	int32 exY = exX;

	const int32 XYunits = workLOD == 0 ? pTerrainConfig->TileXUnits : pTerrainConfig->TileXUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor;
	const int32 exUnitSize = workLOD == 0 ? pTerrainConfig->UnitSize : pTerrainConfig->UnitSize * pTerrainConfig->LODs[workLOD].ResolutionDivisor;

	// Calculate the new noisemap
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			int32 worldX = (((workJob.mySector.X * XYunits) + x) * exUnitSize);
			int32 worldY = (((workJob.mySector.Y * XYunits) + y) * exUnitSize);

			pMeshData->HeightMap[x + (exX*y)] = pTerrainConfig->NoiseGenerator->GetNoise2D(worldX, worldY);

		}
	}
	// Put heightmap into Red channel

	if (pTerrainConfig->GenerateSplatMap && workLOD == 0)
	{
		int i = 0;
		for (int x = 0; x < pTerrainConfig->TileXUnits; ++x)
		{
			for (int y = 0; y < pTerrainConfig->TileYUnits; ++y)
			{
				int32 worldX = (((workJob.mySector.X * pTerrainConfig->TileXUnits) + x) * exUnitSize);
				int32 worldY = (((workJob.mySector.Y * pTerrainConfig->TileYUnits) + y) * exUnitSize);

				float& noiseValue = pMeshData->HeightMap[(x + 1) + (exX*(y + 1))];

				pMeshData->myTextureData[i].R = (uint8)FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(0.0f, 255.0f), noiseValue);

				pMeshData->myTextureData[i].G = (uint8)FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 0.0f), FVector2D(0.0f, 255.0f), noiseValue);

				pMeshData->myTextureData[i].B = i;

				pMeshData->myTextureData[i].A = 0;

				
				i++;
			}
		}
	}

	// Then put the biome map into the Green vertex colour channel
	if (pTerrainConfig->BiomeBlendGenerator)
	{
		exX -= 2;
		exY -= 2;
		for (int x = 0; x < exX; ++x)
		{
			for (int y = 0; y < exY; ++y)
			{
				int32 worldX = (((workJob.mySector.X * (exX - 1)) + x) * exUnitSize);
				int32 worldY = (((workJob.mySector.Y * (exX - 1)) + y) * exUnitSize);
				float val = pTerrainConfig->BiomeBlendGenerator->GetNoise2D(worldX, worldY);

				pMeshData->MyVertexData[x + (exX*y)].Color.G = FMath::Clamp(FMath::RoundToInt(((val + 1.0f) / 2.0f) * 256), 0, 255);
			}
		}
	}

}

void FCGTerrainGeneratorWorker::AddDepositionToHeightMap()
{
	int32 index = 0;
	for (float& heightPoint : pMeshData->HeightMap)
	{
		//heightPoint.Z += (*pDepositionMap)[index];
		++index;
	}
}


void FCGTerrainGeneratorWorker::erodeHeightMapAtIndex(int32 aX, int32 aY, float aAmount)
{
	int32 XUnits = GetNumberOfNoiseSamplePoints();
	float mod1 = 0.5f;
	float mod2 = 0.4f;

	pMeshData->HeightMap[aX + (XUnits * aY)] -= aAmount;
	pMeshData->HeightMap[aX + (XUnits * (aY + 1))]		-= aAmount * mod1;
	pMeshData->HeightMap[aX + (XUnits * (aY - 1))]		-= aAmount * mod1;
	pMeshData->HeightMap[aX + 1 + (XUnits * (aY))]		-= aAmount * mod1;
	pMeshData->HeightMap[aX - 1 + (XUnits * (aY))]		-= aAmount * mod1;

	pMeshData->HeightMap[aX + 1 + (XUnits * (aY + 1))]	-= aAmount * mod1;
	pMeshData->HeightMap[aX + 1 + (XUnits * (aY - 1))]	-= aAmount * mod1;
	pMeshData->HeightMap[aX - 1 + (XUnits * (aY + 1))]	-= aAmount * mod1;
	pMeshData->HeightMap[aX - 1 + (XUnits * (aY - 1))]	-= aAmount * mod1;

	// Add to the Red channel for deposition
	if (aAmount > 0.0f)
	{
		//pMeshData->MyVertexData[aX - 1 + ((XUnits - 2) * (aY - 1))].Color.R = FMath::Clamp(pMeshData->MyVertexData[aX - 1 + ((XUnits - 2) * (aY - 1))].Color.R + FMath::RoundToInt(aAmount), 0, 255);
	}
	// Add to the blue channel for erosion
	if (aAmount <= 0.0f)
	{
		//pMeshData->MyVertexData[aX - 1 + ((XUnits - 2) * (aY - 1))].Color.B = FMath::Clamp(pMeshData->MyVertexData[aX - 1 + ((XUnits - 2) * (aY - 1))].Color.B + FMath::RoundToInt(aAmount * 0.01f), 0, 255);
	}

}

void FCGTerrainGeneratorWorker::ProcessSingleDropletErosion()
{
	int32 XUnits = GetNumberOfNoiseSamplePoints();
	int32 YUnits = XUnits;

	// Pick a random start point that isn't on an edge
	int32 cX = FMath::RandRange(1, XUnits - 1);
	int32 cY = FMath::RandRange(1, YUnits - 1);

	float sedimentAmount = 0.0f;
	float waterAmount	 = 1.0f;
	FVector velocity = FVector(0.0f, 0.0f, 1.0f);
	

	//while (waterAmount > 0.0f && cX > 0 && cX < XUnits - 1 && cY > 0 && cY < YUnits - 1)
	//{
	//	FVector origin = pMeshData->HeightMap[cX + (XUnits * cY)];
	//	if (origin.Z < pTerrainConfig->DropletErosionFloor)
	//	{
	//		// Don't care about underwater erosion
	//		break;
	//	}
	//	FVector up = (pMeshData->HeightMap[cX + (XUnits * (cY + 1))] - origin).GetSafeNormal();
	//	FVector down = (pMeshData->HeightMap[cX + (XUnits * (cY - 1))] - origin).GetSafeNormal();
	//	FVector left = (pMeshData->HeightMap[cX + 1 + (XUnits * (cY))] - origin).GetSafeNormal();
	//	FVector right = (pMeshData->HeightMap[cX - 1 + (XUnits * (cY))] - origin).GetSafeNormal();

	//	FVector upleft = (pMeshData->HeightMap[cX + 1 + (XUnits * (cY + 1))] - origin).GetSafeNormal();
	//	FVector downleft = (pMeshData->HeightMap[cX + 1 + (XUnits * (cY - 1))] - origin).GetSafeNormal();
	//	FVector upright = (pMeshData->HeightMap[cX - 1 + (XUnits * (cY + 1))] - origin).GetSafeNormal();
	//	FVector downright = (pMeshData->HeightMap[cX - 1 + (XUnits * (cY - 1))] - origin).GetSafeNormal();

	//	FVector lowestRoute = FVector(0.0f);

	//	int32 newCx = cX;
	//	int32 newCy = cY;

	//	if (up.Z < lowestRoute.Z) { lowestRoute = up; newCy++; }
	//	if (down.Z < lowestRoute.Z) { lowestRoute = down; newCy--; }
	//	if (left.Z < lowestRoute.Z) { lowestRoute = left; newCx++; }
	//	if (right.Z < lowestRoute.Z) { lowestRoute = right; newCx--; }
	//	if (upleft.Z < lowestRoute.Z) { lowestRoute = upleft; newCy++; newCx++; }
	//	if (upright.Z < lowestRoute.Z) { lowestRoute = upright; newCy++; newCx--; }
	//	if (downleft.Z < lowestRoute.Z) { lowestRoute = downleft; newCy--; newCx++; }
	//	if (downright.Z < lowestRoute.Z) { lowestRoute = downright; newCy--; newCx--; }

	//	// The amount of sediment to pick up depends on if we are hitting an obstacle
	//	float sedimentUptake = pTerrainConfig->DropletErosionMultiplier * FVector::DotProduct(velocity, lowestRoute);
	//	if (sedimentUptake < 0.0f) { sedimentUptake = 0.0f; }

	//	sedimentAmount += sedimentUptake;

	//	float sedimentDeposit = 0.0f;
	//	// Deposit sediment if we are carrying too much
	//	if (sedimentAmount > pTerrainConfig->DropletSedimentCapacity)
	//	{
	//		sedimentDeposit = (sedimentAmount - pTerrainConfig->DropletSedimentCapacity) * pTerrainConfig->DropletDespositionMultiplier;
	//	}

	//	// Deposit based on slope
	//	sedimentDeposit += sedimentAmount * FMath::Clamp(1.0f + lowestRoute.Z, 0.0f, 1.0f);

	//	sedimentAmount -= sedimentDeposit;


	//	velocity = lowestRoute;

	//	erodeHeightMapAtIndex(cX, cY, (sedimentUptake + (sedimentDeposit * -1.0f)));

	//	waterAmount -= pTerrainConfig->DropletEvaporationRate;

	//	cX = newCx;
	//	cY = newCy;

	

}

void FCGTerrainGeneratorWorker::ProcessPerBlockGeometry()
{
	int32 vertCounter = 0;
	int32 triCounter = 0;

	int32 xUnits = workLOD == 0 ? pTerrainConfig->TileXUnits : (pTerrainConfig->TileXUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);
	int32 yUnits = workLOD == 0 ? pTerrainConfig->TileYUnits : (pTerrainConfig->TileYUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);

	// Generate the mesh data for each block
	for (int32 y = 0; y < yUnits; ++y)
	{
		for (int32 x = 0; x < xUnits; ++x)
		{
			UpdateOneBlockGeometry(x, y, vertCounter, triCounter);
		}
	}
}

void FCGTerrainGeneratorWorker::ProcessPerVertexTasks()
{
	SCOPE_CYCLE_COUNTER(STAT_Normals);
	int32 xUnits = workLOD == 0 ? pTerrainConfig->TileXUnits : (pTerrainConfig->TileXUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);
	int32 yUnits = workLOD == 0 ? pTerrainConfig->TileYUnits : (pTerrainConfig->TileYUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);

	int32 rowLength = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);

	for (int32 y = 0; y < yUnits + 1; ++y)
	{
		for (int32 x = 0; x < xUnits + 1; ++x)
		{
			FVector normal;
			FRuntimeMeshTangent tangent;



			GetNormalFromHeightMapForVertex(x, y, normal, tangent);

			uint8 slopeChan = FMath::RoundToInt((1.0f - FMath::Abs(FVector::DotProduct(normal, FVector::UpVector))) * 256) ;
			pMeshData->MyVertexData[x + (y * rowLength)].Color.R = slopeChan;
			pMeshData->MyVertexData[x + (y * rowLength)].SetNormalAndTangent(normal, tangent);
		}
	}
}


// Generates the 'skirt' geometry that falls down from the edges of each tile
void FCGTerrainGeneratorWorker::ProcessSkirtGeometry()
{
	// Going to do this the simple way, keep code easy to understand!

	int32 numXVerts = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1;
	int32 numYVerts = workLOD == 0 ? pTerrainConfig->TileYUnits + 1 : (pTerrainConfig->TileYUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1;

	
	int32 startIndex = numXVerts * numYVerts;
	int32 triStartIndex = ((numXVerts - 1) * (numYVerts - 1) * 6);

	// Bottom Edge verts
	for (int i = 0; i < numXVerts; ++i)
	{
		pMeshData->MyVertexData[startIndex + i].Position.X = pMeshData->MyVertexData[i].Position.X;
		pMeshData->MyVertexData[startIndex + i].Position.Y = pMeshData->MyVertexData[i].Position.Y;
		pMeshData->MyVertexData[startIndex + i].Position.Z = -30000.0f;

		pMeshData->MyVertexData[startIndex + i].Normal = pMeshData->MyVertexData[i].Normal;
	}
	// bottom edge triangles
	for (int i = 0; i < ((numXVerts - 1)); ++i)
	{
		pMeshData->MyTriangles[triStartIndex + (i*6)]		= i;
		pMeshData->MyTriangles[triStartIndex + (i*6) + 1]	= startIndex + i + 1;
		pMeshData->MyTriangles[triStartIndex + (i*6) + 2]	= startIndex + i;

		pMeshData->MyTriangles[triStartIndex + (i*6) + 3]	= i + 1;
		pMeshData->MyTriangles[triStartIndex + (i*6) + 4]	= startIndex + i + 1;
		pMeshData->MyTriangles[triStartIndex + (i*6) + 5]	= i;
	}
	triStartIndex += ((numXVerts - 1) * 6);

	startIndex = ((numXVerts) * (numYVerts + 1));
	// Top Edge verts
	for (int i = 0; i < numXVerts; ++i)
	{
		pMeshData->MyVertexData[startIndex + i].Position.X = pMeshData->MyVertexData[i + startIndex - (numXVerts * 2)].Position.X;
		pMeshData->MyVertexData[startIndex + i].Position.Y = pMeshData->MyVertexData[i + startIndex - (numXVerts * 2)].Position.Y;
		pMeshData->MyVertexData[startIndex + i].Position.Z = -30000.0f;

		pMeshData->MyVertexData[startIndex + i].Normal = pMeshData->MyVertexData[i + startIndex - (numXVerts * 2)].Normal;
	}
	// top edge triangles
	
	for (int i = 0; i < ((numXVerts - 1)); ++i)
	{
		pMeshData->MyTriangles[triStartIndex + (i * 6)]		= i + startIndex - (numXVerts * 2);
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 1]	= startIndex + i;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 2]	= i + startIndex - (numXVerts * 2) + 1;

		pMeshData->MyTriangles[triStartIndex + (i * 6) + 3]	= i + startIndex - (numXVerts * 2) + 1;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 4]	= startIndex + i;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 5]	= startIndex + i + 1;
	}
	triStartIndex += ((numXVerts - 1) * 6);

	startIndex = numXVerts * (numYVerts + 2);
	// Right edge - bit different
	for (int i = 0; i < numYVerts - 2; ++i)
	{
		pMeshData->MyVertexData[startIndex + i].Position.X = pMeshData->MyVertexData[(i + 1) * numXVerts].Position.X;
		pMeshData->MyVertexData[startIndex + i].Position.Y = pMeshData->MyVertexData[(i + 1) * numXVerts].Position.Y;
		pMeshData->MyVertexData[startIndex + i].Position.Z = -30000.0f;

		pMeshData->MyVertexData[startIndex + i].Normal = pMeshData->MyVertexData[(i + 1) * numXVerts].Normal;
	}
	// Bottom right corner
	

	pMeshData->MyTriangles[triStartIndex]		= 0;
	pMeshData->MyTriangles[triStartIndex + 1]	= numXVerts * numYVerts;
	pMeshData->MyTriangles[triStartIndex + 2]	= numXVerts;

	pMeshData->MyTriangles[triStartIndex + 3]	= numXVerts;
	pMeshData->MyTriangles[triStartIndex + 4]	= numXVerts * numYVerts;
	pMeshData->MyTriangles[triStartIndex + 5]	= numXVerts * (numYVerts + 2);

	// Top right corner
	triStartIndex += 6;

	pMeshData->MyTriangles[triStartIndex]		= numXVerts * (numYVerts - 1);
	pMeshData->MyTriangles[triStartIndex + 1]	= (numXVerts * (numYVerts + 2)) + numYVerts - 3;
	pMeshData->MyTriangles[triStartIndex + 2]	= numXVerts * (numYVerts + 1);

	pMeshData->MyTriangles[triStartIndex + 3]	= numXVerts * (numYVerts - 1);
	pMeshData->MyTriangles[triStartIndex + 4]	= numXVerts * (numYVerts - 2);
	pMeshData->MyTriangles[triStartIndex + 5]	= (numXVerts * (numYVerts + 2)) + numYVerts - 3;

	// Middle right part!
	startIndex = numXVerts * (numYVerts + 2);
	triStartIndex += 6;

	for (int i = 0; i < numYVerts - 3; ++i)
	{
		pMeshData->MyTriangles[triStartIndex + (i*6)]		= numXVerts * (i + 1);
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 1]	= startIndex + i;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 2]	= numXVerts * (i + 2);

		pMeshData->MyTriangles[triStartIndex + (i * 6) + 3]	= numXVerts * (i + 2);
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 4]	= startIndex + i;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 5]	= startIndex + i + 1;
	}
	triStartIndex += ((numYVerts - 3) * 6);

	
	startIndex += (numYVerts - 2);
	// Left edge - bit different
	for (int i = 0; i < numYVerts - 2; ++i)
	{
		pMeshData->MyVertexData[startIndex + i].Position.X = pMeshData->MyVertexData[((i + 1) * numXVerts) + numXVerts - 1].Position.X;
		pMeshData->MyVertexData[startIndex + i].Position.Y = pMeshData->MyVertexData[((i + 1) * numXVerts) + numXVerts - 1].Position.Y;
		pMeshData->MyVertexData[startIndex + i].Position.Z = -30000.0f;

		pMeshData->MyVertexData[startIndex + i].Normal = pMeshData->MyVertexData[((i + 1) * numXVerts) + numXVerts - 1].Normal;
	}
	// Bottom left corner


	pMeshData->MyTriangles[triStartIndex]		= numXVerts - 1;
	pMeshData->MyTriangles[triStartIndex + 1]	= (numXVerts*2) - 1;
	pMeshData->MyTriangles[triStartIndex + 2]	= startIndex;

	pMeshData->MyTriangles[triStartIndex + 3]	= startIndex;
	pMeshData->MyTriangles[triStartIndex + 4]	= (numXVerts * numYVerts) + numXVerts - 1;
	pMeshData->MyTriangles[triStartIndex + 5]	= numXVerts - 1;

	// Top left corner
	triStartIndex += 6;

	pMeshData->MyTriangles[triStartIndex]		= (numXVerts * numYVerts) - 1;
	pMeshData->MyTriangles[triStartIndex + 1]	= (numXVerts * (numYVerts + 2)) - 1;
	pMeshData->MyTriangles[triStartIndex + 2]	= (numXVerts * (numYVerts + 2)) + ((numYVerts - 2) * 2) - 1;

	pMeshData->MyTriangles[triStartIndex + 3]	= (numXVerts * numYVerts) - 1;
	pMeshData->MyTriangles[triStartIndex + 4]	= (numXVerts * (numYVerts + 2)) + ((numYVerts - 2) * 2) - 1;
	pMeshData->MyTriangles[triStartIndex + 5]	= (numXVerts * (numYVerts - 2)) + numXVerts - 1;

	// Middle left part!
	
	triStartIndex += 6;

	for (int i = 0; i < numYVerts - 3; ++i)
	{
		pMeshData->MyTriangles[triStartIndex + (i * 6)]		= (numXVerts * (i + 1)) + numXVerts - 1;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 1]	= (numXVerts * (i + 2)) + numXVerts - 1;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 2]	= startIndex + i + 1;

		pMeshData->MyTriangles[triStartIndex + (i * 6) + 3]	= (numXVerts * (i + 1)) + numXVerts - 1;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 4]	= startIndex + i + 1;
		pMeshData->MyTriangles[triStartIndex + (i * 6) + 5]	= startIndex + i;
	}


}

void FCGTerrainGeneratorWorker::GetNormalFromHeightMapForVertex(const int32& vertexX, const int32& vertexY, FVector& aOutNormal, FRuntimeMeshTangent& aOutTangent)
{
	FVector result;

	FVector tangentVec, bitangentVec;

	const int32 rowLength = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);
	const int32 heightMapRowLength = rowLength + 2;

	// the heightmapIndex for this vertex index
	const int32 heightMapIndex = vertexX + 1 + ((vertexY + 1) * heightMapRowLength);
	const float worldTileX = workJob.mySector.X * pTerrainConfig->TileXUnits;
	const float worldTileY = workJob.mySector.Y * pTerrainConfig->TileYUnits;
	const float& unitSize = workLOD == 0 ? pTerrainConfig->UnitSize : pTerrainConfig->UnitSize * pTerrainConfig->LODs[workLOD].ResolutionDivisor;
	const float& ampl = pTerrainConfig->Amplitude;

	FVector origin = FVector((worldTileX + vertexX) * unitSize, (worldTileY + vertexY) * unitSize, pMeshData->HeightMap[heightMapIndex] * ampl);

	// Get the 4 neighbouring points
	FVector up, down, left, right;

	up = FVector((worldTileX + vertexX) * unitSize, (worldTileY + vertexY + 1) * unitSize, pMeshData->HeightMap[heightMapIndex + heightMapRowLength] * ampl) - origin;
	down = FVector((worldTileX + vertexX) * unitSize, (worldTileY + vertexY - 1) * unitSize, pMeshData->HeightMap[heightMapIndex - heightMapRowLength] * ampl) - origin;
	left = FVector((worldTileX + vertexX + 1) * unitSize, (worldTileY + vertexY) * unitSize, pMeshData->HeightMap[heightMapIndex + 1] * ampl) - origin;
	right = FVector((worldTileX + vertexX - 1) * unitSize, (worldTileY + vertexY) * unitSize, pMeshData->HeightMap[heightMapIndex - 1] * ampl) - origin;


	FVector n1, n2, n3, n4;

	n1 = FVector::CrossProduct(left, up);
	n2 = FVector::CrossProduct(up, right);
	n3 = FVector::CrossProduct(right, down);
	n4 = FVector::CrossProduct(down, left);

	result = n1 + n2 + n3 + n4;

	aOutNormal = result.GetSafeNormal();

	// We can mega cheap out here as we're dealing with a simple flat grid
	aOutTangent = FRuntimeMeshTangent(left.GetSafeNormal(), false);
}


void FCGTerrainGeneratorWorker::UpdateOneBlockGeometry(const int32& aX, const int32& aY, int32& aVertCounter, int32& triCounter)
{
	int32 thisX = aX;
	int32 thisY = aY;
	int32 heightMapX = thisX + 1;
	int32 heightMapY = thisY + 1;
	// LOD adjusted dimensions
	int32 rowLength = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);
	int32 heightMapRowLength = rowLength + 2;
	// LOD adjusted unit size
	int32 exUnitSize = workLOD == 0 ? pTerrainConfig->UnitSize : pTerrainConfig->UnitSize * (pTerrainConfig->LODs[workLOD].ResolutionDivisor);

	const int blockX = 0;
	const int blockY = 0;
	const float& unitSize = pTerrainConfig->UnitSize;
	const float& ampl = pTerrainConfig->Amplitude;

	FVector heightMapToWorldOffset = FVector(0.0f, 0.0f, 0.0f);

	// TL
	pMeshData->MyVertexData[thisX + (thisY * rowLength)].Position = FVector((blockX + thisX) * exUnitSize, (blockY + thisY) * exUnitSize, pMeshData->HeightMap[heightMapX + (heightMapY * heightMapRowLength)] * ampl) -heightMapToWorldOffset;
	// TR
	pMeshData->MyVertexData[thisX + ((thisY + 1) * rowLength)].Position = FVector((blockX + thisX) * exUnitSize, (blockY + thisY + 1) * exUnitSize, pMeshData->HeightMap[heightMapX + ((heightMapY + 1) * heightMapRowLength)] * ampl) -heightMapToWorldOffset;
	// BL
	pMeshData->MyVertexData[(thisX + 1) + (thisY * rowLength)].Position = FVector((blockX + thisX + 1) * exUnitSize, (blockY + thisY) * exUnitSize, pMeshData->HeightMap[(heightMapX + 1) + (heightMapY * heightMapRowLength)] * ampl) -heightMapToWorldOffset;
	// BR
	pMeshData->MyVertexData[(thisX + 1) + ((thisY + 1) * rowLength)].Position = FVector((blockX + thisX + 1) * exUnitSize, (blockY + thisY + 1) * exUnitSize, pMeshData->HeightMap[(heightMapX + 1) + ((heightMapY + 1) * heightMapRowLength)] * ampl) -heightMapToWorldOffset;
}

int32 FCGTerrainGeneratorWorker::GetNumberOfNoiseSamplePoints()
{
	return workLOD == 0 ? pTerrainConfig->TileXUnits + 3 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor)) + 3;
}
