

#include "cashgen.h"
#include "UFNNoiseGenerator.h"
#include "FZoneGeneratorWorker.h"

FZoneGeneratorWorker::FZoneGeneratorWorker(AZoneManager*		apZoneManager,
	FZoneConfig*			aZoneConfig,
	CGPoint*				aOffSet,
	TMap<uint8, eLODStatus>*   aZoneJobData,
	uint8 aLOD,
	TArray<FVector>*	aVertices,
	TArray<int32>*		aTriangles,
	TArray<FVector>*	aNormals,
	TArray<FVector2D>*	aUV0,
	TArray<FColor>*		aVertexColors,
	TArray<FRuntimeMeshTangent>* aTangents,
	TArray<FVector>* aHeightMap,
	TArray<FBiomeWeights>* aBiomeWeightMap)
{
	pCallingZoneManager = apZoneManager;
	pOffset = aOffSet;
	pZoneJobData = aZoneJobData;
	MyLOD = aLOD;
	pZoneConfig = aZoneConfig;
	pVertices = aVertices;
	pTriangles = aTriangles;
	pNormals = aNormals;
	pUV0 = aUV0;
	pVertexColors = aVertexColors;
	pTangents = aTangents;
	pHeightMap = aHeightMap;
	pBiomeWeightMap = aBiomeWeightMap;
}

FZoneGeneratorWorker::~FZoneGeneratorWorker()
{

}

bool FZoneGeneratorWorker::Init()
{
	return true;
}

uint32 FZoneGeneratorWorker::Run()
{
	ProcessTerrainMap();

	ProcessPerBlockGeometry();

	ProcessPerVertexTasks();

	if (MyLOD == 0)
	{
		ProcessBiomeWeightMap();
	}

	if (pCallingZoneManager->MyLODMeshStatus[MyLOD] == eLODStatus::BUILDING_REQUIRES_CREATE) {
		pCallingZoneManager->MyLODMeshStatus[MyLOD] = eLODStatus::READY_TO_DRAW_REQUIRES_CREATE;
	}
	else if(pCallingZoneManager->MyLODMeshStatus[MyLOD] == eLODStatus::BUILDING)
	{
		pCallingZoneManager->MyLODMeshStatus[MyLOD] = eLODStatus::READY_TO_DRAW;
	}

	return 1;
}

void FZoneGeneratorWorker::Stop()
{
	
}

void FZoneGeneratorWorker::Exit()
{
	
}

/************************************************************************/
/* Builds the heightmap structure using the noise parameters            */
/************************************************************************/
void FZoneGeneratorWorker::ProcessTerrainMap()
{
	MyMaxHeight = 0.0f;

	// Size of the noise sampling (larger than the actual mesh so we can have seamless normals)
	int32 exX = GetNumberOfNoiseSamplePoints();
	int32 exY = exX;

	int32 exUnitSize = MyLOD == 0 ? pZoneConfig->UnitSize : pZoneConfig->UnitSize * (FMath::Pow(2, MyLOD));

	// Calculate the new noisemap
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			int32 worldX = (((pOffset->x * (exX - 3) + x)) * exUnitSize) - exUnitSize;
			int32 worldY = (((pOffset->y * (exY - 3) + y)) * exUnitSize) - exUnitSize;
			
			(*pHeightMap)[x + (exX*y)] = FVector(x* exUnitSize, y*exUnitSize, pZoneConfig->noiseGen->GetNoise2D(worldX, worldY) * pZoneConfig->Amplitude);
		}
	} 
}

// Calculates the normals and tangents for each vertex
void FZoneGeneratorWorker::ProcessPerVertexTasks()
{
	int32 xUnits = MyLOD == 0 ? pZoneConfig->XUnits : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD)));
	int32 yUnits = MyLOD == 0 ? pZoneConfig->YUnits : (pZoneConfig->YUnits / (FMath::Pow(2, MyLOD)));
	int32 rowLength = MyLOD == 0 ? pZoneConfig->XUnits + 1 : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD)) + 1);

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

void FZoneGeneratorWorker::ProcessBiomeWeightMap()
{
	int32 xUnits = MyLOD == 0 ? pZoneConfig->XUnits : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD)));
	int32 yUnits = MyLOD == 0 ? pZoneConfig->YUnits : (pZoneConfig->YUnits / (FMath::Pow(2, MyLOD)));

	for (int32 y = 0; y < yUnits; ++y)
	{
		for (int32 x = 0; x < xUnits; ++x)
		{
			if ((*pHeightMap)[(x+1) + ((y+1)* (pZoneConfig->YUnits + 3))].Z > pZoneConfig->ShoreHeight && (*pHeightMap)[(x + 1) + ((y + 1)* (pZoneConfig->YUnits + 3))].Z <= pZoneConfig->TreeLine)
			{
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Trees, 0.5f));
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Grass, 1.0f));
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Rocks, 0.1f));
			}
			else if ((*pHeightMap)[(x + 1) + ((y + 1)* (pZoneConfig->YUnits + 3))].Z > pZoneConfig->TreeLine)
			{
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Trees, 0.1f));
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Grass, 0.3f));
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Rocks, 1.0f));
			}
			else
			{
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Trees, 0.0f));
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Grass, 0.0f));
				(*pBiomeWeightMap)[x + (y * xUnits)].BiomeWeights.Add(FBiomeWeight(EBiome::Rocks, 0.0f));
			}
			
		}
	}

}

// Builds the geometry for the mesh
void FZoneGeneratorWorker::ProcessPerBlockGeometry()
{
	int32 vertCounter = 0;
	int32 triCounter = 0;

	int32 xUnits = MyLOD == 0 ? pZoneConfig->XUnits : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD)));
	int32 yUnits = MyLOD == 0 ? pZoneConfig->YUnits : (pZoneConfig->YUnits / (FMath::Pow(2, MyLOD)));

	// Generate the mesh data for each block
	for (int32 y = 0; y < yUnits; ++y)
	{
		for (int32 x = 0; x < xUnits; ++x)
		{
			UpdateOneBlockGeometry(x, y, vertCounter, triCounter);
		}
	}
}

// Updates the vertices and other data for a single block (two tris)
void FZoneGeneratorWorker::UpdateOneBlockGeometry(const int aX, const int aY, int32& aVertCounter, int32& triCounter)
{
	int32 thisX = aX;
	int32 thisY = aY;
	int32 heightMapX = thisX + 1;
	int32 heightMapY = thisY + 1;
	// LOD adjusted dimensions
	int32 rowLength = MyLOD == 0 ? pZoneConfig->XUnits + 1 : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD)) + 1);
	int32 heightMapRowLength = rowLength + 2;
	// LOD adjusted unit size
	int32 exUnitSize = MyLOD == 0 ? pZoneConfig->UnitSize : pZoneConfig->UnitSize * (FMath::Pow(2, MyLOD));

	FVector heightMapToWorldOffset = FVector(exUnitSize, exUnitSize, 0.0f);

	// BR
	(*pVertices)[thisX + (thisY * rowLength)]				= (*pHeightMap)[heightMapX + (heightMapY * heightMapRowLength)] - heightMapToWorldOffset;
	// TR
	(*pVertices)[thisX + ((thisY + 1) * rowLength)]			= (*pHeightMap)[heightMapX + ((heightMapY + 1) * heightMapRowLength)] - heightMapToWorldOffset;
	// BL
	(*pVertices)[(thisX + 1) + (thisY * rowLength)]			= (*pHeightMap)[(heightMapX + 1) + (heightMapY * heightMapRowLength)] - heightMapToWorldOffset;
	// BR
	(*pVertices)[(thisX + 1) + ((thisY + 1) * rowLength)]	= (*pHeightMap)[(heightMapX + 1) + ((heightMapY + 1) * heightMapRowLength)] - heightMapToWorldOffset;

	//TODO: This isn't doing anything at the moment 
	//(*pVertexColors)[thisX + (thisY * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[thisX + ((thisY + 1) * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[(thisX + 1) + (thisY * rowLength)].R = (255 / 50000.0f);
	//(*pVertexColors)[(thisX + 1) + ((thisY + 1) * rowLength)].R = (255 / 50000.0f);

}

int32 FZoneGeneratorWorker::GetNumberOfNoiseSamplePoints()
{
	return MyLOD == 0 ? pZoneConfig->XUnits + 3 : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD))) + 3;
}

// Erm, I think this fudge works
FRuntimeMeshTangent FZoneGeneratorWorker::GetTangentFromNormal(const FVector aNormal)
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

	return FRuntimeMeshTangent(bitangentVec, false );
}

// Gets a smoothed normal based on the 8 neighbouring vertices
FVector FZoneGeneratorWorker::GetNormalFromHeightMapForVertex(const int32 vertexX, const int32 vertexY)
{
	FVector result;

	int32 rowLength = MyLOD == 0 ? pZoneConfig->XUnits + 1 : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD)) + 1);
	int32 heightMapRowLength = rowLength + 2;

	// the heightmapIndex for this vertex index
	int32 heightMapIndex = vertexX + 1 + ((vertexY + 1) * heightMapRowLength);

	// Get the 4 neighbouring points
	FVector up, down, left, right, upleft, upright, downleft, downright;

	up		= (*pHeightMap)[heightMapIndex + heightMapRowLength] - (*pHeightMap)[heightMapIndex];
	down	= (*pHeightMap)[heightMapIndex - heightMapRowLength] - (*pHeightMap)[heightMapIndex];
	left	= (*pHeightMap)[heightMapIndex + 1] - (*pHeightMap)[heightMapIndex];
	right	= (*pHeightMap)[heightMapIndex - 1] - (*pHeightMap)[heightMapIndex];

	FVector n1, n2, n3, n4;

	n1 = FVector::CrossProduct(left, up);
	n2 = FVector::CrossProduct(up, right);
	n3 = FVector::CrossProduct(right, down);
	n4 = FVector::CrossProduct(down, left);

	result = n1 + n2 + n3 + n4;

	return result.GetSafeNormal();
}
