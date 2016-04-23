#pragma once
#include "cashgenUE.h"
#include "MeshConfig.h"
#include "BiomeConfig.generated.h"

USTRUCT()
struct FBiomeConfig
{
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Config Struct")
		TArray<FMeshConfig> MeshConfigs;
};