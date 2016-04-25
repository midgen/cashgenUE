

#include "cashgen.h"
#include "FZoneGeneratorWorker.h"
#include "UnrealLibNoise.h"
#include "KismetProceduralMeshLibrary.h"

FZoneGeneratorWorker::FZoneGeneratorWorker(AZoneManager*		apZoneManager,
	FZoneConfig*			aZoneConfig,
	Point*				aOffSet,
	TMap<uint8, eLODStatus>*   aZoneJobData,
	uint8 aLOD,
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
	pZoneJobData = aZoneJobData;
	MyLOD = aLOD;
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

	ProcessChildMeshSpawns();

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(*pVertices, *pTriangles, *pUV0, *pNormals, *pTangents);

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

void FZoneGeneratorWorker::ProcessTerrainMap()
{
	MyMaxHeight = 0.0f;

	int32 exX = MyLOD == 0 ? pZoneConfig->XUnits + 2 : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD))) + 2;
	int32 exY = MyLOD == 0 ? pZoneConfig->YUnits + 2 : (pZoneConfig->YUnits / (FMath::Pow(2, MyLOD))) + 2;

	int32 exUnitSize = MyLOD == 0 ? pZoneConfig->UnitSize : pZoneConfig->UnitSize * (FMath::Pow(2, MyLOD));

	// Calculate the new noisemap
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			int32 thisX = (pOffset->x * (exX - 2) + x) * exUnitSize;
			int32 thisY = (pOffset->y * (exY - 2) + y) * exUnitSize;
			(*pZoneData)[x].blocks[y].Height = pZoneConfig->noiseModule->GetValue(FVector(thisX, thisY, 0.0f)) * pZoneConfig->Amplitude;
		}
	}

	// Now run through and calculate vertex heights
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			(*pZoneData)[x].blocks[y].ProcessCorners(MyMaxHeight, exUnitSize);

		}
	}
}

void FZoneGeneratorWorker::ProcessChildMeshSpawns()
{
	int32 exX = MyLOD == 0 ? pZoneConfig->XUnits + 2 : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD))) + 2;
	int32 exY = MyLOD == 0 ? pZoneConfig->YUnits + 2 : (pZoneConfig->YUnits / (FMath::Pow(2, MyLOD))) + 2;
	int32 exUnitSize = MyLOD == 0 ? pZoneConfig->UnitSize : pZoneConfig->UnitSize * (FMath::Pow(2, MyLOD));

	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			int32 thisWorldX = (pOffset->x * (exX - 2) + x) * exUnitSize;
			int32 thisWorldY = (pOffset->y * (exY - 2) + y) * exUnitSize;

			float biomeMaskValue = pZoneConfig->biomeMask->GetValue(FVector(thisWorldX, thisWorldY, 0.0f));

			// Mountains
			if (biomeMaskValue < pZoneConfig->BiomeMaskThreshold)
			{
				if ((*pZoneData)[x].blocks[y].Slope < pZoneConfig->FlatMaxSlope)
				{
					(*pZoneData)[x].blocks[y].Biome = EBiome::MountainFlat;
				}
				else if ((*pZoneData)[x].blocks[y].Slope < pZoneConfig->SlopeMaxSlope)
				{
					(*pZoneData)[x].blocks[y].Biome = EBiome::MountainSlope;
				}
				else
				{
					(*pZoneData)[x].blocks[y].Biome = EBiome::MountainCliff;
				}
			}
			// Plains and coast
			else
			{
				if ((*pZoneData)[x].blocks[y].Height < 0.0f)
				{
					(*pZoneData)[x].blocks[y].Biome = EBiome::Underwater;
				}
				else if ((*pZoneData)[x].blocks[y].Height < pZoneConfig->ShoreLineHeight)
				{
					(*pZoneData)[x].blocks[y].Biome = EBiome::Shoreline;
				}
				else if ((*pZoneData)[x].blocks[y].Height < pZoneConfig->CoastLineHeight)
				{
					(*pZoneData)[x].blocks[y].Biome = EBiome::Coast;
				}
				else
				{
					if ((*pZoneData)[x].blocks[y].Slope < pZoneConfig->FlatMaxSlope)
					{
						(*pZoneData)[x].blocks[y].Biome = EBiome::PlainFlat;
					}
					else if ((*pZoneData)[x].blocks[y].Slope < pZoneConfig->SlopeMaxSlope)
					{
						(*pZoneData)[x].blocks[y].Biome = EBiome::PlainSlope;
					}
					else
					{
						(*pZoneData)[x].blocks[y].Biome = EBiome::PlainCliff;
					}
				}

			}
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

	int32 lodX = MyLOD == 0 ? pZoneConfig->XUnits + 1 : (pZoneConfig->XUnits / (FMath::Pow(2, MyLOD)) + 1);

	int32 exUnitSize = MyLOD == 0 ? pZoneConfig->UnitSize : pZoneConfig->UnitSize * (FMath::Pow(2, MyLOD));

	int32 blockOffSetAmount = exUnitSize * 0.5f;
	int32 zoneOffSetAmount = exUnitSize * 0.5f;


	(*pVertices)[thisX + (thisY * lodX)].X = (aBlock->MyX * exUnitSize) - blockOffSetAmount - zoneOffSetAmount;
	(*pVertices)[thisX + (thisY * lodX)].Y = (aBlock->MyY * exUnitSize) - blockOffSetAmount - zoneOffSetAmount;
	(*pVertices)[thisX + (thisY * lodX)].Z = aBlock->bottomRightCorner.height;

	(*pVertices)[thisX + ((thisY + 1) * lodX)].X = (aBlock->MyX * exUnitSize) - blockOffSetAmount - zoneOffSetAmount;
	(*pVertices)[thisX + ((thisY + 1) * lodX)].Y = (aBlock->MyY * exUnitSize) + blockOffSetAmount - zoneOffSetAmount;
	(*pVertices)[thisX + ((thisY + 1) * lodX)].Z = aBlock->topRightCorner.height;

	(*pVertices)[(thisX + 1) + (thisY * lodX)].X = (aBlock->MyX * exUnitSize) + blockOffSetAmount - zoneOffSetAmount;
	(*pVertices)[(thisX + 1) + (thisY * lodX)].Y = (aBlock->MyY * exUnitSize) - blockOffSetAmount - zoneOffSetAmount;
	(*pVertices)[(thisX + 1) + (thisY * lodX)].Z = aBlock->bottomLeftCorner.height;

	(*pVertices)[(thisX + 1) + ((thisY + 1) * lodX)].X = (aBlock->MyX * exUnitSize) + blockOffSetAmount - zoneOffSetAmount;
	(*pVertices)[(thisX + 1) + ((thisY + 1) * lodX)].Y = (aBlock->MyY * exUnitSize) + blockOffSetAmount - zoneOffSetAmount;
	(*pVertices)[(thisX + 1) + ((thisY + 1) * lodX)].Z = aBlock->topLeftCorner.height;

	(*pVertexColors)[thisX + (thisY * lodX)].R = (255 / 50000.0f) * aBlock->Height;
	(*pVertexColors)[thisX + ((thisY + 1) * lodX)].R = (255 / 50000.0f) * aBlock->Height;
	(*pVertexColors)[(thisX + 1) + (thisY * lodX)].R = (255 / 50000.0f) * aBlock->Height;
	(*pVertexColors)[(thisX + 1) + ((thisY + 1) * lodX)].R = (255 / 50000.0f) * aBlock->Height;


}


FVector FZoneGeneratorWorker::CalcSurfaceNormalForTriangle(FVector v1, FVector v2, FVector v3)
{
	FVector U = v2 - v1;
	FVector V = v3 - v1;

	return FVector::CrossProduct(V, U).GetSafeNormal();
}

