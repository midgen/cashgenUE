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
	UPROPERTY()
		UUFNNoiseGenerator* BiomeBlendGenerator;
	/** Number of blocks along a zone's X axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Main")
		int32 TileXUnits;
	/** Number of blocks along a zone's Y axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Main")
		int32 TileYUnits;
	/** Size of a single block in world units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Main")
		float UnitSize;
	/** Multiplier for heightmap*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "cashGen | Main")
		float Amplitude;
	/** Droplet erosion droplet amount *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Erosion")
		int32 DropletAmount;
	/** Droplet erosion deposition rate *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Erosion")
		float DropletErosionMultiplier;
	/** Droplet erosion deposition rate *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Erosion")
		float DropletDespositionMultiplier;
	/** Droplet erosion deposition Theta *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Erosion")
		float DropletSedimentCapacity;
	/** Droplet erosion evaporation rate *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Erosion")
		float DropletEvaporationRate;
	/** Material for the terrain mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Erosion")
	UMaterial* TerrainMaterialInstanceParent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | LODs")
	TArray<FCGLODConfig> LODs;

};