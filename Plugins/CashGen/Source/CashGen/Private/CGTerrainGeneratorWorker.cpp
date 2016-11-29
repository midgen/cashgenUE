#include "cashgen.h"
#include "UFNNoiseGenerator.h"
#include "CGTerrainGeneratorWorker.h"




FCGTerrainGeneratorWorker::FCGTerrainGeneratorWorker(ACGTerrainManager* aTerrainManager, FCGTerrainConfig* aTerrainConfig)
{
	pTerrainManager = aTerrainManager;
	pTerrainConfig = aTerrainConfig;
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
		// If there's a job, process it!
		if (pTerrainManager->GeometryJobs.Dequeue(workJob))
		{
			//pMeshData = workJob.Data;

			pVertices = workJob.Vertices;
			pTriangles = workJob.Triangles;
			pNormals = workJob.Normals;
			pUV0 = workJob.UV0;
			pVertexColors = workJob.VertexColors;
			pTangents = workJob.Tangents;
			pHeightMap = workJob.HeightMap;

			workLOD = workJob.LOD;

			ProcessTerrainMap();
			ProcessPerBlockGeometry();
			ProcessPerVertexTasks();


			pTerrainManager->UpdateJobs.Enqueue(workJob);
		}
		// Otherwise, take a nap
		else
		{
			Sleep(30);
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
			int32 worldX = (((workJob.Tile->Offset.X * (exX - 3) + x)) * exUnitSize) - exUnitSize;
			int32 worldY = (((workJob.Tile->Offset.Y * (exY - 3) + y)) * exUnitSize) - exUnitSize;

			(*pHeightMap)[x + (exX*y)] = FVector(x* exUnitSize, y*exUnitSize, pTerrainConfig->NoiseGenerator->GetNoise2D(worldX, worldY) * pTerrainConfig->Amplitude);
		}
	}
}

void FCGTerrainGeneratorWorker::ProcessPerBlockGeometry()
{
	int32 vertCounter = 0;
	int32 triCounter = 0;

	int32 xUnits = workLOD == 0 ? pTerrainConfig->XUnits : (pTerrainConfig->XUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);
	int32 yUnits = workLOD == 0 ? pTerrainConfig->YUnits : (pTerrainConfig->YUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);

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
	int32 xUnits = workLOD == 0 ? pTerrainConfig->XUnits : (pTerrainConfig->XUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);
	int32 yUnits = workLOD == 0 ? pTerrainConfig->YUnits : (pTerrainConfig->YUnits / pTerrainConfig->LODs[workLOD].ResolutionDivisor);

	int32 rowLength = workLOD == 0 ? pTerrainConfig->XUnits + 1 : (pTerrainConfig->XUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);

	for (int32 y = 0; y < yUnits + 1; ++y)
	{
		for (int32 x = 0; x < xUnits + 1; ++x)
		{
			(*pNormals)[x + (y * rowLength)] = GetNormalFromHeightMapForVertex(x, y);
			// TODO: Pretty sure this is wrong, so out it goes for now
			//(*pTangents)[x + (y * rowLength)] = GetTangentFromNormal((*pNormals)[x + (y * rowLength)]);
		}
	}
}

void FCGTerrainGeneratorWorker::ProcessBiomeWeightMap()
{

}

FVector FCGTerrainGeneratorWorker::GetNormalFromHeightMapForVertex(const int32 vertexX, const int32 vertexY)
{
	FVector result;

	int32 rowLength = workLOD == 0 ? pTerrainConfig->XUnits + 1 : (pTerrainConfig->XUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);
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
	int32 rowLength = workLOD == 0 ? pTerrainConfig->XUnits + 1 : (pTerrainConfig->XUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor) + 1);
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

	//TODO: Not using Vertex Colour channels at the moment, could be handy though!

	//(*pVertexColors)[thisX + (thisY * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[thisX + ((thisY + 1) * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[(thisX + 1) + (thisY * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[(thisX + 1) + ((thisY + 1) * rowLength)].R = (255 / 50000.0f);
}

int32 FCGTerrainGeneratorWorker::GetNumberOfNoiseSamplePoints()
{
	//return workLOD == 0 ? pTerrainConfig->XUnits + 3 : (pTerrainConfig->XUnits / (FMath::Pow(2, workLOD))) + 3;
	return workLOD == 0 ? pTerrainConfig->XUnits + 3 : (pTerrainConfig->XUnits / (pTerrainConfig->LODs[workLOD].ResolutionDivisor)) + 3;
}
