#include "cashgen.h"
#include "UFNNoiseGenerator.h"
#include "CGTerrainGeneratorWorker.h"
#include <chrono>

using namespace std::chrono;


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
			pVertices = workJob.Vertices;
			pTriangles = workJob.Triangles;
			pNormals = workJob.Normals;
			pUV0 = workJob.UV0;
			pVertexColors = workJob.VertexColors;
			pTangents = workJob.Tangents;
			pHeightMap = workJob.HeightMap;
			pDepositionMap = workJob.DespositionMap;

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
				for (int32 i = 0; i < pTerrainConfig->DropletAmount; ++i)
				{
					ProcessSingleDropletErosion();
				}
			}

			workJob.ErosionGenerationDuration = (duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				) - startMs).count();

			ProcessPerBlockGeometry();
			ProcessPerVertexTasks();
			ProcessSkirtGeometry();


			pTerrainManager->UpdateJobs.Enqueue(workJob);
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
	for (int32 i = 0; i < pVertexColors->Num(); ++i)
	{
		(*pVertexColors)[i].R = 0;
		(*pVertexColors)[i].G = 0;
		(*pVertexColors)[i].B = 0;
		(*pVertexColors)[i].A = 0;
	}
}

void FCGTerrainGeneratorWorker::ProcessTerrainMap()
{
	// Size of the noise sampling (larger than the actual mesh so we can have seamless normals)
	int32 exX = GetNumberOfNoiseSamplePoints();
	int32 exY = exX;

	int32 exUnitSize = workLOD == 0 ? pTerrainConfig->UnitSize : pTerrainConfig->UnitSize * pTerrainConfig->LODs[workLOD].ResolutionDivisor;

	// Calculate the new noisemap
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			int32 worldX = (((workJob.Tile->Offset.X * (exX - 3)) + x) * exUnitSize);
			int32 worldY = (((workJob.Tile->Offset.Y * (exX - 3)) + y) * exUnitSize);

			(*pHeightMap)[x + (exX*y)] = FVector(x* exUnitSize, y*exUnitSize, pTerrainConfig->NoiseGenerator->GetNoise2D(worldX, worldY) * pTerrainConfig->Amplitude);

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
				int32 worldX = (((workJob.Tile->Offset.X * (exX - 1)) + x) * exUnitSize);
				int32 worldY = (((workJob.Tile->Offset.Y * (exX - 1)) + y) * exUnitSize);
				float val = pTerrainConfig->BiomeBlendGenerator->GetNoise2D(worldX, worldY);

				(*pVertexColors)[x + (exX*y)].G = FMath::Clamp(FMath::FloorToInt(((val + 1.0f) / 2.0f) * 128), 0, 255);
			}
		}
	}

}

void FCGTerrainGeneratorWorker::AddDepositionToHeightMap()
{
	int32 index = 0;
	for (FVector& heightPoint : (*pHeightMap))
	{
		heightPoint.Z += (*pDepositionMap)[index];
		++index;
	}
}


void FCGTerrainGeneratorWorker::erodeHeightMapAtIndex(int32 aX, int32 aY, float aAmount)
{
	int32 XUnits = GetNumberOfNoiseSamplePoints();
	float mod1 = 0.5f;
	float mod2 = 0.4f;

	(*pHeightMap)[aX + (XUnits * aY)].Z -= aAmount;
	(*pHeightMap)[aX + (XUnits * (aY + 1))].Z		-= aAmount * mod1;
	(*pHeightMap)[aX + (XUnits * (aY - 1))].Z		-= aAmount * mod1;
	(*pHeightMap)[aX + 1 + (XUnits * (aY))].Z		-= aAmount * mod1;
	(*pHeightMap)[aX - 1 + (XUnits * (aY))].Z		-= aAmount * mod1;

	(*pHeightMap)[aX + 1 + (XUnits * (aY + 1))].Z	-= aAmount * mod1;
	(*pHeightMap)[aX + 1 + (XUnits * (aY - 1))].Z	-= aAmount * mod1;
	(*pHeightMap)[aX - 1 + (XUnits * (aY + 1))].Z	-= aAmount * mod1;
	(*pHeightMap)[aX - 1 + (XUnits * (aY - 1))].Z	-= aAmount * mod1;

	// Add to the Red channel for deposition
	if (aAmount > 0.0f)
	{
		(*pVertexColors)[aX - 1 + ((XUnits - 2) * (aY - 1))].R = FMath::Clamp((*pVertexColors)[aX - 1 + ((XUnits - 2) * (aY - 1))].R + FMath::RoundToInt(aAmount), 0, 255);
	}
	// Add to the blue channel for erosion
	if (aAmount <= 0.0f)
	{
		(*pVertexColors)[aX - 1 + ((XUnits - 2) * (aY - 1))].B = FMath::Clamp((*pVertexColors)[aX - 1 + ((XUnits - 2) * (aY - 1))].B + FMath::RoundToInt(aAmount * 0.01f), 0, 255);
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
	

	while (waterAmount > 0.0f && cX > 0 && cX < XUnits - 1 && cY > 0 && cY < YUnits - 1)
	{
		FVector origin = (*pHeightMap)[cX + (XUnits * cY)];
		if (origin.Z < pTerrainConfig->DropletErosionFloor)
		{
			// Don't care about underwater erosion
			break;
		}
		FVector up = ((*pHeightMap)[cX + (XUnits * (cY + 1))] - origin).GetSafeNormal();
		FVector down = ((*pHeightMap)[cX + (XUnits * (cY - 1))] - origin).GetSafeNormal();
		FVector left = ((*pHeightMap)[cX + 1 + (XUnits * (cY))] - origin).GetSafeNormal();
		FVector right = ((*pHeightMap)[cX - 1 + (XUnits * (cY))] - origin).GetSafeNormal();
		
		FVector upleft = ((*pHeightMap)[cX + 1 + (XUnits * (cY + 1))] - origin).GetSafeNormal();
		FVector downleft = ((*pHeightMap)[cX + 1 + (XUnits * (cY - 1))] - origin).GetSafeNormal();
		FVector upright = ((*pHeightMap)[cX - 1 + (XUnits * (cY + 1))] - origin).GetSafeNormal();
		FVector downright = ((*pHeightMap)[cX - 1 + (XUnits * (cY - 1))] - origin).GetSafeNormal();

		FVector lowestRoute = FVector(0.0f);

		int32 newCx = cX;
		int32 newCy = cY;

		if (up.Z < lowestRoute.Z) { lowestRoute = up; newCy++; }
		if (down.Z < lowestRoute.Z) { lowestRoute = down; newCy--; }
		if (left.Z < lowestRoute.Z) { lowestRoute = left; newCx++; }
		if (right.Z < lowestRoute.Z) { lowestRoute = right; newCx--; }
		if (upleft.Z < lowestRoute.Z) { lowestRoute = upleft; newCy++; newCx++; }
		if (upright.Z < lowestRoute.Z) { lowestRoute = upright; newCy++; newCx--; }
		if (downleft.Z < lowestRoute.Z) { lowestRoute = downleft; newCy--; newCx++; }
		if (downright.Z < lowestRoute.Z) { lowestRoute = downright; newCy--; newCx--; }

		FVector2D direction = FVector2D(newCx, newCy);
		direction.X += velocity.X;
		direction.Y += velocity.Y;
		direction.Normalize();

		// The amount of sediment to pick up depends on if we are hitting an obstacle
		float sedimentUptake = pTerrainConfig->DropletErosionMultiplier * FVector::DotProduct(velocity, lowestRoute);
		if (sedimentUptake < 0.0f) { sedimentUptake = 0.0f; }

		sedimentAmount += sedimentUptake;

		float sedimentDeposit = 0.0f;
		// Deposit sediment if we are carrying too much
		if (sedimentAmount > pTerrainConfig->DropletSedimentCapacity)
		{
			sedimentDeposit = (sedimentAmount - pTerrainConfig->DropletSedimentCapacity) * pTerrainConfig->DropletDespositionMultiplier;
		}

		// Deposit based on slope
		sedimentDeposit += sedimentAmount * FMath::Clamp(1.0f + lowestRoute.Z, 0.0f, 1.0f);

		sedimentAmount -= sedimentDeposit;
		
		float descent = lowestRoute.Z;
		// Average this velocity with previous
		velocity = (velocity + lowestRoute) * 0.5f;

		direction = direction.ClampAxes(1.0f, XUnits - 1.0f);

		erodeHeightMapAtIndex(FMath::RoundHalfFromZero(direction.X), FMath::RoundHalfFromZero(direction.Y), (sedimentUptake + (sedimentDeposit * -1.0f)));
		
		waterAmount -= pTerrainConfig->DropletEvaporationRate;

		cX = newCx;
		cY = newCy;

	}

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
	int32 xUnits = workLOD == 0 ? pTerrainConfig->TileXUnits : (pTerrainConfig->TileXUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);
	int32 yUnits = workLOD == 0 ? pTerrainConfig->TileYUnits : (pTerrainConfig->TileYUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);

	int32 rowLength = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);

	for (int32 y = 0; y < yUnits + 1; ++y)
	{
		for (int32 x = 0; x < xUnits + 1; ++x)
		{
			(*pNormals)[x + (y * rowLength)] = GetNormalFromHeightMapForVertex(x, y);
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
		(*pVertices)[startIndex + i].X = (*pVertices)[i].X;
		(*pVertices)[startIndex + i].Y = (*pVertices)[i].Y;
		(*pVertices)[startIndex + i].Z = -30000.0f;

		(*pNormals)[startIndex + i] = (*pNormals)[i];
	}
	// bottom edge triangles
	for (int i = 0; i < ((numXVerts - 1)); ++i)
	{
		(*pTriangles)[triStartIndex + (i*6)]		= i;
		(*pTriangles)[triStartIndex + (i*6) + 1]	= startIndex + i + 1;
		(*pTriangles)[triStartIndex + (i*6) + 2]	= startIndex + i;

		(*pTriangles)[triStartIndex + (i*6) + 3]	= i + 1;
		(*pTriangles)[triStartIndex + (i*6) + 4]	= startIndex + i + 1;
		(*pTriangles)[triStartIndex + (i*6) + 5]	= i;
	}
	triStartIndex += ((numXVerts - 1) * 6);

	startIndex = ((numXVerts) * (numYVerts + 1));
	// Top Edge verts
	for (int i = 0; i < numXVerts; ++i)
	{
		(*pVertices)[startIndex + i].X = (*pVertices)[i + startIndex - (numXVerts * 2)].X;
		(*pVertices)[startIndex + i].Y = (*pVertices)[i + startIndex - (numXVerts * 2)].Y;
		(*pVertices)[startIndex + i].Z = -30000.0f;

		(*pNormals)[startIndex + i] = (*pNormals)[i + startIndex - (numXVerts * 2)];
	}
	// top edge triangles
	
	for (int i = 0; i < ((numXVerts - 1)); ++i)
	{
		(*pTriangles)[triStartIndex + (i * 6)]		= i + startIndex - (numXVerts * 2);
		(*pTriangles)[triStartIndex + (i * 6) + 1]	= startIndex + i;
		(*pTriangles)[triStartIndex + (i * 6) + 2]	= i + startIndex - (numXVerts * 2) + 1;

		(*pTriangles)[triStartIndex + (i * 6) + 3]	= i + startIndex - (numXVerts * 2) + 1;
		(*pTriangles)[triStartIndex + (i * 6) + 4]	= startIndex + i;
		(*pTriangles)[triStartIndex + (i * 6) + 5]	= startIndex + i + 1;
	}
	triStartIndex += ((numXVerts - 1) * 6);

	startIndex = numXVerts * (numYVerts + 2);
	// Right edge - bit different
	for (int i = 0; i < numYVerts - 2; ++i)
	{
		(*pVertices)[startIndex + i].X = (*pVertices)[(i + 1) * numXVerts].X;
		(*pVertices)[startIndex + i].Y = (*pVertices)[(i + 1) * numXVerts].Y;
		(*pVertices)[startIndex + i].Z = -30000.0f;

		(*pNormals)[startIndex + i] = (*pNormals)[(i + 1) * numXVerts];
	}
	// Bottom right corner
	

	(*pTriangles)[triStartIndex]		= 0;
	(*pTriangles)[triStartIndex + 1]	= numXVerts * numYVerts;
	(*pTriangles)[triStartIndex + 2]	= numXVerts;

	(*pTriangles)[triStartIndex + 3]	= numXVerts;
	(*pTriangles)[triStartIndex + 4]	= numXVerts * numYVerts;
	(*pTriangles)[triStartIndex + 5]	= numXVerts * (numYVerts + 2);

	// Top right corner
	triStartIndex += 6;

	(*pTriangles)[triStartIndex]		= numXVerts * (numYVerts - 1);
	(*pTriangles)[triStartIndex + 1]	= (numXVerts * (numYVerts + 2)) + numYVerts - 3;
	(*pTriangles)[triStartIndex + 2]	= numXVerts * (numYVerts + 1);

	(*pTriangles)[triStartIndex + 3]	= numXVerts * (numYVerts - 1);
	(*pTriangles)[triStartIndex + 4]	= numXVerts * (numYVerts - 2);
	(*pTriangles)[triStartIndex + 5]	= (numXVerts * (numYVerts + 2)) + numYVerts - 3;

	// Middle right part!
	startIndex = numXVerts * (numYVerts + 2);
	triStartIndex += 6;

	for (int i = 0; i < numYVerts - 3; ++i)
	{
		(*pTriangles)[triStartIndex + (i*6)]		= numXVerts * (i + 1);
		(*pTriangles)[triStartIndex + (i * 6) + 1]	= startIndex + i;
		(*pTriangles)[triStartIndex + (i * 6) + 2]	= numXVerts * (i + 2);

		(*pTriangles)[triStartIndex + (i * 6) + 3]	= numXVerts * (i + 2);
		(*pTriangles)[triStartIndex + (i * 6) + 4]	= startIndex + i;
		(*pTriangles)[triStartIndex + (i * 6) + 5]	= startIndex + i + 1;
	}
	triStartIndex += ((numYVerts - 3) * 6);

	
	startIndex += (numYVerts - 2);
	// Left edge - bit different
	for (int i = 0; i < numYVerts - 2; ++i)
	{
		(*pVertices)[startIndex + i].X = (*pVertices)[((i + 1) * numXVerts) + numXVerts - 1].X;
		(*pVertices)[startIndex + i].Y = (*pVertices)[((i + 1) * numXVerts) + numXVerts - 1].Y;
		(*pVertices)[startIndex + i].Z = -30000.0f;

		(*pNormals)[startIndex + i] = (*pNormals)[((i + 1) * numXVerts) + numXVerts - 1];
	}
	// Bottom left corner


	(*pTriangles)[triStartIndex]		= numXVerts - 1;
	(*pTriangles)[triStartIndex + 1]	= (numXVerts*2) - 1;
	(*pTriangles)[triStartIndex + 2]	= startIndex;

	(*pTriangles)[triStartIndex + 3]	= startIndex;
	(*pTriangles)[triStartIndex + 4]	= (numXVerts * numYVerts) + numXVerts - 1;
	(*pTriangles)[triStartIndex + 5]	= numXVerts - 1;

	// Top left corner
	triStartIndex += 6;

	(*pTriangles)[triStartIndex]		= (numXVerts * numYVerts) - 1;
	(*pTriangles)[triStartIndex + 1]	= (numXVerts * (numYVerts + 2)) - 1;
	(*pTriangles)[triStartIndex + 2]	= (numXVerts * (numYVerts + 2)) + ((numYVerts - 2) * 2) - 1;

	(*pTriangles)[triStartIndex + 3]	= (numXVerts * numYVerts) - 1;
	(*pTriangles)[triStartIndex + 4]	= (numXVerts * (numYVerts + 2)) + ((numYVerts - 2) * 2) - 1;
	(*pTriangles)[triStartIndex + 5]	= (numXVerts * (numYVerts - 2)) + numXVerts - 1;

	// Middle left part!
	
	triStartIndex += 6;

	for (int i = 0; i < numYVerts - 3; ++i)
	{
		(*pTriangles)[triStartIndex + (i * 6)]		= (numXVerts * (i + 1)) + numXVerts - 1;
		(*pTriangles)[triStartIndex + (i * 6) + 1]	= (numXVerts * (i + 2)) + numXVerts - 1;
		(*pTriangles)[triStartIndex + (i * 6) + 2]	= startIndex + i + 1;

		(*pTriangles)[triStartIndex + (i * 6) + 3]	= (numXVerts * (i + 1)) + numXVerts - 1;
		(*pTriangles)[triStartIndex + (i * 6) + 4]	= startIndex + i + 1;
		(*pTriangles)[triStartIndex + (i * 6) + 5]	= startIndex + i;
	}


}

FVector FCGTerrainGeneratorWorker::GetNormalFromHeightMapForVertex(const int32 vertexX, const int32 vertexY)
{
	FVector result;

	int32 rowLength = workLOD == 0 ? pTerrainConfig->TileXUnits + 1 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);
	int32 heightMapRowLength = rowLength + 2;

	// the heightmapIndex for this vertex index
	int32 heightMapIndex = vertexX + 1 + ((vertexY + 1) * heightMapRowLength);

	// Get the 4 neighbouring points
	FVector up, down, left, right, upleft, upright, downleft, downright;

	up = (*pHeightMap)[heightMapIndex + heightMapRowLength] - (*pHeightMap)[heightMapIndex];
	down = (*pHeightMap)[heightMapIndex - heightMapRowLength] - (*pHeightMap)[heightMapIndex];
	left = (*pHeightMap)[heightMapIndex + 1] - (*pHeightMap)[heightMapIndex];
	right = (*pHeightMap)[heightMapIndex - 1] - (*pHeightMap)[heightMapIndex];

	FVector n1, n2, n3, n4;

	n1 = FVector::CrossProduct(left, up);
	n2 = FVector::CrossProduct(up, right);
	n3 = FVector::CrossProduct(right, down);
	n4 = FVector::CrossProduct(down, left);

	result = n1 + n2 + n3 + n4;

	return result.GetSafeNormal();
}

FRuntimeMeshTangent FCGTerrainGeneratorWorker::GetTangentFromNormal(const FVector aNormal)
{
	FVector tangentVec, bitangentVec;
	FVector c1, c2;

	c1 = FVector::CrossProduct(aNormal, FVector(0.0f, 0.0f, 1.0f));
	c2 = FVector::CrossProduct(aNormal, FVector(0.0f, 1.0f, 0.0f));

	if (c1.Size() > c2.Size())
	{
		tangentVec = c1;
	}
	else
	{
		tangentVec = c2;
	}

	tangentVec = tangentVec.GetSafeNormal();
	bitangentVec = FVector::CrossProduct(aNormal, tangentVec);

	return FRuntimeMeshTangent(bitangentVec, false);
}

void FCGTerrainGeneratorWorker::UpdateOneBlockGeometry(const int aX, const int aY, int32& aVertCounter, int32& triCounter)
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

	FVector heightMapToWorldOffset = FVector(exUnitSize, exUnitSize, 0.0f);

	// BR
	(*pVertices)[thisX + (thisY * rowLength)] = (*pHeightMap)[heightMapX + (heightMapY * heightMapRowLength)] - heightMapToWorldOffset;
	// TR
	(*pVertices)[thisX + ((thisY + 1) * rowLength)] = (*pHeightMap)[heightMapX + ((heightMapY + 1) * heightMapRowLength)] - heightMapToWorldOffset;
	// BL
	(*pVertices)[(thisX + 1) + (thisY * rowLength)] = (*pHeightMap)[(heightMapX + 1) + (heightMapY * heightMapRowLength)] - heightMapToWorldOffset;
	// BR
	(*pVertices)[(thisX + 1) + ((thisY + 1) * rowLength)] = (*pHeightMap)[(heightMapX + 1) + ((heightMapY + 1) * heightMapRowLength)] - heightMapToWorldOffset;
}

int32 FCGTerrainGeneratorWorker::GetNumberOfNoiseSamplePoints()
{
	return workLOD == 0 ? pTerrainConfig->TileXUnits + 3 : (pTerrainConfig->TileXUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor)) + 3;
}
