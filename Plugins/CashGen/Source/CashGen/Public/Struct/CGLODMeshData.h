#pragma once
#include "CGLODMeshData.generated.h"

USTRUCT()
struct FCGLODMeshData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCGMeshData> Data;

};