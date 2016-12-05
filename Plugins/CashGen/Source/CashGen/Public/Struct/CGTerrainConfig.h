#pragma once
#include "cashgen.h"
#include "UFNNoiseGenerator.h"
#include "Struct/CGLODConfig.h"
#include "UFNBlendModule.h"
#include "CGTerrainConfig.generated.h"


/** Struct defines all applicable attributes for managing generation of a single zone */
USTRUCT()
struct FCGTerrainConfig
{
	GENERATED_USTRUCT_BODY()
		/** Noise Generator configuration struct */
	UPROPERTY(EditAnywhere, Category = "Zone Config Struct")
		UUFNNoiseGenerator* NoiseGenerator;
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
	UMaterial* TerrainMaterialInstanceParent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCGLODConfig> LODs;
};