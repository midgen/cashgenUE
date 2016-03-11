#include "cashgenUE.h"
#include "FZoneGeneratorWorker.h"
#include "PerlinNoise.h"
#include "KismetProceduralMeshLibrary.h"


FZoneGeneratorWorker::FZoneGeneratorWorker(AZoneManager*		apZoneManager,
	ZoneConfig*			aZoneConfig,
	Point*				aOffSet,
	TArray<GridRow>*	aZoneData,
	TArray<FVector>*	aVertices,
	TArray<int32>*		aTriangles,
	TArray<FVector>*	aNormals,
	TArray<FVector2D>*	aUV0,
	TArray<FColor>*		aVertexColors,
	TArray<FProcMeshTangent>* aTangents)
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
	pTangents = aTangents;

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

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(*pVertices, *pTriangles, *pUV0, *pNormals, *pTangents);

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
			if ((*pZoneData)[x].blocks[y].Height > pZoneConfig->FloorHeight - pZoneConfig->FloorDepth && (*pZoneData)[x].blocks[y].Height < pZoneConfig->FloorHeight + pZoneConfig->FloorDepth)
			{
				(*pZoneData)[x].blocks[y].Height = (*pZoneData)[x].blocks[y].Height * 0.2f;
			}
			else {
				(*pZoneData)[x].blocks[y].Height -= pZoneConfig->FloorHeight;
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
	int32 vertCounter = 0;
	int32 triCounter = 0;
	// Generate the mesh data
	for (int32 y = 0; y < (*pZoneData)[0].blocks.Num() - 2; ++y)	
	{
		for (int32 x = 0; x < pZoneData->Num() - 2; ++x)
		{
			UpdateOneBlockGeometry(&(*pZoneData)[x + 1].blocks[y + 1], vertCounter, triCounter);
		}
	}
}

void FZoneGeneratorWorker::UpdateOneBlockGeometry(ZoneBlock* aBlock, int32& aVertCounter, int32& triCounter)
{
	int32 thisX = aBlock->MyX - 1; 
	int32 thisY = aBlock->MyY - 1;


	(*pVertices)[thisX + (thisY * (pZoneConfig->XUnits + 1))].X = (aBlock->MyX * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[thisX + (thisY * (pZoneConfig->XUnits + 1))].Y = (aBlock->MyY * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[thisX + (thisY * (pZoneConfig->XUnits + 1))].Z = aBlock->bottomRightCorner.height;
	
	(*pVertices)[thisX + ((thisY + 1) * (pZoneConfig->XUnits + 1))].X = (aBlock->MyX * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[thisX + ((thisY + 1) * (pZoneConfig->XUnits + 1))].Y = (aBlock->MyY * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[thisX + ((thisY + 1) * (pZoneConfig->XUnits + 1))].Z = aBlock->topRightCorner.height;

	(*pVertices)[(thisX + 1) + (thisY * (pZoneConfig->XUnits + 1))].X = (aBlock->MyX * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[(thisX + 1) + (thisY * (pZoneConfig->XUnits + 1))].Y = (aBlock->MyY * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
	(*pVertices)[(thisX + 1) + (thisY * (pZoneConfig->XUnits + 1))].Z = aBlock->bottomLeftCorner.height;
	
	(*pVertices)[(thisX + 1) + ((thisY + 1) * (pZoneConfig->XUnits + 1))].X = (aBlock->MyX * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[(thisX + 1) + ((thisY + 1) * (pZoneConfig->XUnits + 1))].Y = (aBlock->MyY * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
	(*pVertices)[(thisX + 1) + ((thisY + 1) * (pZoneConfig->XUnits + 1))].Z = aBlock->topLeftCorner.height;

	
}


//
//void FZoneGeneratorWorker::UpdateOneBlockGeometry(ZoneBlock* aBlock, int32 aVertCounter)
//{
//	(*pVertices)[aVertCounter].X = (aBlock->MyX * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter].Y = (aBlock->MyY * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter].Z = aBlock->bottomLeftCorner.height;
//
//	(*pVertices)[aVertCounter + 1].X = (aBlock->MyX * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 1].Y = (aBlock->MyY * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 1].Z = aBlock->bottomRightCorner.height;
//
//	(*pVertices)[aVertCounter + 2].X = (aBlock->MyX * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 2].Y = (aBlock->MyY * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 2].Z = aBlock->topLeftCorner.height;
//
//	(*pVertices)[aVertCounter + 3].X = (aBlock->MyX * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 3].Y = (aBlock->MyY * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 3].Z = aBlock->bottomRightCorner.height;
//
//	(*pVertices)[aVertCounter + 4].X = (aBlock->MyX * pZoneConfig->UnitSize) - (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 4].Y = (aBlock->MyY * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 4].Z = aBlock->topRightCorner.height;
//
//	(*pVertices)[aVertCounter + 5].X = (aBlock->MyX * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 5].Y = (aBlock->MyY * pZoneConfig->UnitSize) + (pZoneConfig->UnitSize*0.5);
//	(*pVertices)[aVertCounter + 5].Z = aBlock->topLeftCorner.height;
//
//	FVector t1Normal = CalcSurfaceNormalForTriangle((*pVertices)[aVertCounter],
//														(*pVertices)[aVertCounter + 1],
//														(*pVertices)[aVertCounter + 2]);
//
//	FVector t2Normal = CalcSurfaceNormalForTriangle((*pVertices)[aVertCounter +3],
//														(*pVertices)[aVertCounter + 4],
//														(*pVertices)[aVertCounter + 5]);
//
//
//	(*pNormals)[aVertCounter] = t1Normal;
//	(*pNormals)[aVertCounter + 1] = t1Normal;
//	(*pNormals)[aVertCounter + 2] = t1Normal;
//	(*pNormals)[aVertCounter + 3] = t2Normal;
//	(*pNormals)[aVertCounter + 4] = t2Normal;
//	(*pNormals)[aVertCounter + 5] = t2Normal;
//
//	(*pVertexColors)[aVertCounter] = aBlock->Color;
//	(*pVertexColors)[aVertCounter + 1] = aBlock->Color;
//	(*pVertexColors)[aVertCounter + 2] = aBlock->Color;
//	(*pVertexColors)[aVertCounter + 3] = aBlock->Color;
//	(*pVertexColors)[aVertCounter + 4] = aBlock->Color;
//	(*pVertexColors)[aVertCounter + 5] = aBlock->Color;
//
//}

FVector FZoneGeneratorWorker::CalcSurfaceNormalForTriangle(FVector v1, FVector v2, FVector v3)
{
	FVector U = v2 - v1;
	FVector V = v3 - v1;

	return FVector::CrossProduct(V, U).GetSafeNormal();
}

