#pragma once

#include "CGJob.generated.h"

USTRUCT()
struct FCGJob
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACGTile* Tile;
	
	FCGMeshData* Data;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	uint8 LOD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	bool IsInPlaceUpdate;
};