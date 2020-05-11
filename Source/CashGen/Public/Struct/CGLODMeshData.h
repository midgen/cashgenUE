#pragma once

#include "CashGen/Public/Struct/CGMeshData.h"

#include "CGLODMeshData.generated.h"

USTRUCT(BlueprintType)
struct FCGLODMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	TArray<FCGMeshData> Data;

};