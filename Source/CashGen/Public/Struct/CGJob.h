#pragma once

#include "CashGen/Public/Struct/IntVector2.h"
#include "CashGen/Public/Struct/CGTileHandle.h"
#include "CashGen/Public/CGObjectPool.h"

#include "CGJob.generated.h"

struct FCGMeshData;

USTRUCT(BlueprintType)
struct FCGJob
{
	GENERATED_BODY()


	FIntVector2 mySector;
	FCGTileHandle myTileHandle;
	TCGBorrowedObject<FCGMeshData> Data;
	int32 HeightmapGenerationDuration;
	int32 ErosionGenerationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	uint8 LOD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	bool IsInPlaceUpdate;
};