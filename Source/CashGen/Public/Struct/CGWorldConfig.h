#pragma once
#include "cashgen.h"
#include "UFNNoiseGenerator.h"
#include "CGWorldConfig.generated.h"


/** Struct defines all applicable attributes for managing generation of a single world */
USTRUCT()
struct FCGWorldConfig
{
	GENERATED_USTRUCT_BODY()
	/** Noise Generator configuration struct */
	UPROPERTY()
	UUFNNoiseGenerator* NoiseGenerator;

	/** Number of blocks along a zone's X axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Main")
		float Radius = 5000.0f;
	/** Number of blocks along a zone's Y axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Main")
		int32 Subdivisions = 1;
	/** Multiplier for heightmap*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "cashGen | Main")
		float Amplitude = 5000.0f;
	/** Material for the terrain mesh */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Erosion")
		//UMaterial* TerrainMaterialInstanceParent;


};