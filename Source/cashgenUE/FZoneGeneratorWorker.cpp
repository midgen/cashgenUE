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
			if ((*pZoneData)[x].blocks[y].Height < pZoneConfig->Floor)
			{
				(*pZoneData)[x].blocks[y].Height = 0;
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
	int32 blockCounter = 0;
	// Generate the mesh data
	for (int32 x = 0; x < pZoneData->Num() - 2; ++x)
	{
		for (int32 y = 0; y < (*pZoneData)[x].blocks.Num() - 2; ++y)
		{
			UpdateOneBlockGeometry(&(*pZoneData)[x].blocks[y], blockCounter);
			blockCounter++;
		}
	}
}

void FZoneGeneratorWorker::UpdateOneBlockGeometry(ZoneBlock* aBlock, int32 aBlockCounter)
{
	(*pVertices)[aBlockCounter].X = (pZoneConfig->XUnits * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter].Y = (pZoneConfig->YUnits * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter].Z = aBlock->bottomLeftCorner.height;

	(*pVertices)[aBlockCounter +1].X = (pZoneConfig->XUnits * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter +1].Y = (pZoneConfig->YUnits * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter +1].Z = aBlock->bottomRightCorner.height;

	(*pVertices)[aBlockCounter + 2].X = (pZoneConfig->XUnits * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter + 2].Y = (pZoneConfig->YUnits * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter + 2].Z = aBlock->topLeftCorner.height;

	(*pVertices)[aBlockCounter + 3].X = (pZoneConfig->XUnits * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter + 3].Y = (pZoneConfig->YUnits * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter + 3].Z = aBlock->bottomRightCorner.height;

	(*pVertices)[aBlockCounter + 4].X = (pZoneConfig->XUnits * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter + 4].Y = (pZoneConfig->YUnits * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter + 4].Z = aBlock->topRightCorner.height;

	(*pVertices)[aBlockCounter + 5].X = (pZoneConfig->XUnits * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter + 5].Y = (pZoneConfig->YUnits * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[aBlockCounter + 5].Z = aBlock->topLeftCorner.height;

	FVector t1Normal = CalcSurfaceNormalForTriangle((*pVertices)[aBlockCounter],
														(*pVertices)[aBlockCounter + 1],
														(*pVertices)[aBlockCounter + 2]);

	FVector t2Normal = CalcSurfaceNormalForTriangle((*pVertices)[aBlockCounter+3],
														(*pVertices)[aBlockCounter + 4],
														(*pVertices)[aBlockCounter + 5]);


	(*pNormals)[aBlockCounter] = t1Normal;
	(*pNormals)[aBlockCounter + 1] = t1Normal;
	(*pNormals)[aBlockCounter + 2] = t1Normal;
	(*pNormals)[aBlockCounter + 3] = t2Normal;
	(*pNormals)[aBlockCounter + 4] = t2Normal;
	(*pNormals)[aBlockCounter + 5] = t2Normal;

	(*pVertexColors)[aBlockCounter] = aBlock->Color;
	(*pVertexColors)[aBlockCounter + 1] = aBlock->Color;
	(*pVertexColors)[aBlockCounter + 2] = aBlock->Color;
	(*pVertexColors)[aBlockCounter + 3] = aBlock->Color;
	(*pVertexColors)[aBlockCounter + 4] = aBlock->Color;
	(*pVertexColors)[aBlockCounter + 5] = aBlock->Color;

}

FVector FZoneGeneratorWorker::CalcSurfaceNormalForTriangle(FVector v1, FVector v2, FVector v3)
{
	FVector U = v2 - v1;
	FVector V = v3 - v1;

	return FVector::CrossProduct(V, U).GetSafeNormal();
}

