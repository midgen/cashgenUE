#pragma once

#include "BiomeWeight.generated.h"

UENUM(BlueprintType)
enum class EBiome : uint8 
{
	Trees		UMETA(DisplayName="Trees"),
	Grass		UMETA(DisplayName="Grass"),
	Rocks		UMETA(DisplayName="Rocks")
};

USTRUCT()
struct FBiomeWeight
{
	GENERATED_USTRUCT_BODY()
	FBiomeWeight(const EBiome aBiome, const float aWeight) : Biome(aBiome), Weight(aWeight) {};
	FBiomeWeight() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Weight Struct")
	EBiome Biome;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome Weight Struct")
	float Weight;
};

