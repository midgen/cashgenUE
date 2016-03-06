#include "cashgenUE.h"
#include "FZoneGeneratorWorker.h"
#include "PerlinNoise.h"


FZoneGeneratorWorker::FZoneGeneratorWorker(AZoneManager*		apZoneManager,
	ZoneConfig*			aZoneConfig,
	Point*				aOffSet,
	TArray<GridRow>*	aZoneData,
	TArray<FVector>*	aVertices,
	TArray<int32>*		aTriangles,
	TArray<FVector>*	aNormals,
	TArray<FVector2D>*	aUV0,
	TArray<FColor>*		aVertexColors)
{
	pCallingZoneManager = apZoneManager;
	pOffset = aOffSet;
	pZoneConfig = aZoneConfig;
	pZoneData = aZoneData;
	pVertices = aVertices;
	pTriangles = aTriangles;
	pNormals = aNormals;
	pUV0 = aUV0;
	pVertexColors = aVertexColors;

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

	ProcessGeometry();

	pCallingZoneManager->workerThreadCompleted = true;

	return 1;
}

void FZoneGeneratorWorker::Stop()
{

}

void FZoneGeneratorWorker::Exit()
{

}

void FZoneGeneratorWorker::ProcessTerrainMap()
{
	PerlinNoise noiseGen(pZoneConfig->Persistence, pZoneConfig->Frequency, pZoneConfig->Amplitude, pZoneConfig->Octaves, pZoneConfig->RandomSeed);
	float MyMaxHeight = 0.0f;

	int32 exX = pZoneConfig->XUnits + 2;
	int32 exY = pZoneConfig->YUnits + 2;

	// Calculate the new noisemap
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			int32 thisX = (pOffset->x * pZoneConfig->XUnits) + x;
			int32 thisY = (pOffset->y * pZoneConfig->YUnits) + y;
			(*pZoneData)[x].blocks[y].Height = noiseGen.GetHeight(thisX, thisY);
		}
	}

	

	// Floor pass
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			if ((*pZoneData)[x].blocks[y].Height < pZoneConfig->Floor && (*pZoneData)[x].blocks[y].Height > pZoneConfig->Floor - 2000.0f)
			{
				(*pZoneData)[x].blocks[y].Height = (*pZoneData)[x].blocks[y].Height * 0.2f;
			}
			else {
				(*pZoneData)[x].blocks[y].Height -= pZoneConfig->Floor;
			}

			if ((*pZoneData)[x].blocks[y].Height > MyMaxHeight)
			{
				MyMaxHeight = (*pZoneData)[x].blocks[y].Height;
			}
		}
	}

	// Now run through and calculate vertex heights
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			(*pZoneData)[x].blocks[y].ProcessCorners(MyMaxHeight);
		}
	}
}

void FZoneGeneratorWorker::ProcessGeometry()
{
	int32 triCounter = 0;
	// Generate the mesh data
	for (int32 x = 0; x < pZoneData->Num() - 2; ++x)
	{
		for (int32 y = 0; y < (*pZoneData)[x].blocks.Num() - 2; ++y)
		{
			UpdateOneBlockGeometry(&(*pZoneData)[x + 1].blocks[y + 1], triCounter);
			triCounter += 6;
		}
	}
}

void FZoneGeneratorWorker::UpdateOneBlockGeometry(ZoneBlock* aBlock, int32 aTriCounter)
{
	(*pVertices)[aTriCounter].X = (aBlock->MyX * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter].Y = (aBlock->MyY * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter].Z = aBlock->bottomLeftCorner.height;

	(*pVertices)[aTriCounter + 1].X = (aBlock->MyX * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 1].Y = (aBlock->MyY * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 1].Z = aBlock->bottomRightCorner.height;

	(*pVertices)[aTriCounter + 2].X = (aBlock->MyX * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 2].Y = (aBlock->MyY * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 2].Z = aBlock->topLeftCorner.height;

	(*pVertices)[aTriCounter + 3].X = (aBlock->MyX * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 3].Y = (aBlock->MyY * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 3].Z = aBlock->bottomRightCorner.height;

	(*pVertices)[aTriCounter + 4].X = (aBlock->MyX * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 4].Y = (aBlock->MyY * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 4].Z = aBlock->topRightCorner.height;

	(*pVertices)[aTriCounter + 5].X = (aBlock->MyX * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 5].Y = (aBlock->MyY * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aTriCounter + 5].Z = aBlock->topLeftCorner.height;

	FVector t1Normal = CalcSurfaceNormalForTriangle((*pVertices)[aTriCounter],
														(*pVertices)[aTriCounter + 1],
														(*pVertices)[aTriCounter + 2]);

	FVector t2Normal = CalcSurfaceNormalForTriangle((*pVertices)[aTriCounter +3],
														(*pVertices)[aTriCounter + 4],
														(*pVertices)[aTriCounter + 5]);


	(*pNormals)[aTriCounter] = t1Normal;
	(*pNormals)[aTriCounter + 1] = t1Normal;
	(*pNormals)[aTriCounter + 2] = t1Normal;
	(*pNormals)[aTriCounter + 3] = t2Normal;
	(*pNormals)[aTriCounter + 4] = t2Normal;
	(*pNormals)[aTriCounter + 5] = t2Normal;

	(*pVertexColors)[aTriCounter] = aBlock->Color;
	(*pVertexColors)[aTriCounter + 1] = aBlock->Color;
	(*pVertexColors)[aTriCounter + 2] = aBlock->Color;
	(*pVertexColors)[aTriCounter + 3] = aBlock->Color;
	(*pVertexColors)[aTriCounter + 4] = aBlock->Color;
	(*pVertexColors)[aTriCounter + 5] = aBlock->Color;

}

FVector FZoneGeneratorWorker::CalcSurfaceNormalForTriangle(FVector v1, FVector v2, FVector v3)
{
	FVector U = v2 - v1;
	FVector V = v3 - v1;

	return FVector::CrossProduct(V, U).GetSafeNormal();
}

