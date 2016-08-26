#pragma once

#include "BiomeWeight.h"
#include "BiomeWeights.generated.h"

USTRUCT()
struct FBiomeWeights
{
	GENERATED_USTRUCT_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Weight Struct")
		TArray<FBiomeWeight> BiomeWeights;
};