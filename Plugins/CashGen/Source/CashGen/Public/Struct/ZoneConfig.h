#pragma once
#include "cashgen.h"
#include "BiomeConfig.h"
#include "NoiseConfig.h"
#include "ZoneConfig.generated.h"


/** Struct defines all applicable attributes for managing generation of a single zone */
USTRUCT()
struct FZoneConfig
{
	GENERATED_USTRUCT_BODY()
	/** Noise Generator configuration struct */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	FNoiseConfig noiseConfig;
	/** Number of blocks along a zone's X axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		int32 XUnits;
	/** Number of blocks along a zone's Y axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		int32 YUnits;
	/** Size of a single block in world units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		float UnitSize;
	/** Multiplier for heightmap*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		float Amplitude;
	/** Material for the terrain mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		UMaterial* TerrainMaterial;
	/** LOD 0->1 transition range **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LOD0Range;
	/** LOD 1->2 transition range **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LOD1Range;
	/** LOD 2->3 transition range **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LOD2Range;
	/** Do no render range **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LOD3Range;
};