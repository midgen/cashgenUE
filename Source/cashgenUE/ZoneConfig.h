#pragma once
#include "cashgenUE.h"
#include "BiomeConfig.h"
#include "ZoneBlock.h"
#include "ZoneConfig.generated.h"

/** Struct defines all applicable attributes for managing generation of a single zone */
USTRUCT()
struct FZoneConfig
{
	GENERATED_USTRUCT_BODY()
	/** Number of blocks along a zone's X axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		int32 XUnits;
	/** Number of blocks along a zone's Y axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		int32 YUnits;
	/** Size of a single block in world units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		float UnitSize;
	/** Perlin Noise Generator Attribute - Persistence */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		float Persistence;
	/** Perlin Noise Generator Attribute - Frequency */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		float Frequency;
	/** Perlin Noise Generator Attribute - Amplitude */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		float Amplitude;
	/** Perlin Noise Generator Attribute - Octaves */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		int32 Octaves;
	/** Perlin Noise Generator Attribute - Seed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		int32 RandomSeed;
	/** Max world height for spawning trees */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float TreeLineTop;
	/** Min world height for spawning trees */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float TreeLineBottom;
	/** Degree of slope above which becomes Cliff*/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float CliffSlopeThreshold;
	/** Degree of slope below which becomes woodland */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float WoodSlopeThreshold;
	/** Degree of slope below which becomes grassland */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float GrassSlopeThreshold;
	/** Height that shoreline extends to */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float ShoreLineHeight;
	/** Material for the terrain mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		UMaterial* TerrainMaterial;
	/** Array of Biome configurations - A Biome is a collection of static meshes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
		TArray<FBiomeConfig> BiomeConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LOD0Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LOD1Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LOD2Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LOD3Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LODCullRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	float LODDropDistance;
};