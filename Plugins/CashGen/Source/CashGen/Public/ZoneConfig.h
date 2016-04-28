#pragma once
#include "cashgen.h"
#include "BiomeConfig.h"
#include "NoiseModule.h"
#include "ZoneConfig.generated.h"

/** Struct defines all applicable attributes for managing generation of a single zone */
USTRUCT()
struct FZoneConfig
{
	GENERATED_USTRUCT_BODY()
	/** Reference to the noiseModule for the final heightmap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	TWeakObjectPtr<UNoiseModule> noiseModule;
	/** Reference to the noiseModule for the biome mask  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Config Struct")
	TWeakObjectPtr<UNoiseModule> biomeMask;
	/** Threshold for biome mask*/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
	float BiomeMaskThreshold;
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
	/** Max world height for spawning trees */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float TreeLine;
	/** Degree of slope below which is categorised 'flat'*/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float FlatMaxSlope;
	/** Degree of slope below which is categorised 'slope' */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float SlopeMaxSlope;
	/** Height that shoreline extends to */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float ShoreLineHeight;
	/** Height that Coast extends to */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Zone Config Struct")
		float CoastLineHeight;
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