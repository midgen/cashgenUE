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
	UPROPERTY()
		UUFNNoiseGenerator* NoiseGenerator;
	/** Number of blocks along a zone's X axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		int32 TileXUnits;
	/** Number of blocks along a zone's Y axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		int32 TileYUnits;
	/** Size of a single block in world units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		float UnitSize;
	/** Multiplier for heightmap*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "cashGen")
		float Amplitude;
	/** Thermal erosion iterations *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		int32 ThermalErosionIterations;
	/** Thermal erosion threshold *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		int32 ThermalErosionThreshold;
	/** Thermal erosion deposition amount *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		float ThermalErosionDepositionAmount;
	/** Droplet erosion droplet amount *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		int32 DropletAmount;
	/** Droplet erosion deposition rate *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		float DropletDespositionMultiplier;
	/** Droplet erosion deposition Theta *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		float DropletSedimentCapacity;
	/** Droplet erosion evaporation rate *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
		float DropletEvaporationRate;
	/** Material for the terrain mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	UMaterial* TerrainMaterialInstanceParent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCGLODConfig> LODs;
};