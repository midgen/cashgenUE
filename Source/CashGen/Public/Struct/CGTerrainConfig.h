#pragma once

#include "CashGen/Public/Struct/CGLODConfig.h"

#include <UnrealFastNoisePlugin/Public/UFNBlendModule.h>
#include <UnrealFastNoisePlugin/Public/UFNNoiseGenerator.h>

#include "CGTerrainConfig.generated.h"

/** Struct defines all applicable attributes for managing generation of a single zone */
USTRUCT(BlueprintType)
struct FCGTerrainConfig
{
	GENERATED_BODY()

	FCGTerrainConfig()
		: NoiseGenerator(nullptr)
		, BiomeBlendGenerator(nullptr)
		, WaterMaterialInstance(nullptr)
	{
	}

	/** Noise Generator configuration struct */
	UPROPERTY()
	UUFNNoiseGenerator* NoiseGenerator = nullptr;
	UPROPERTY()
	UUFNNoiseGenerator* BiomeBlendGenerator = nullptr;
	/** Use ASync collision cooking for terrain mesh (Recommended) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|System")
	bool UseAsyncCollision = true;
	/** Size of MeshData pool */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|System")
	uint8 MeshDataPoolSize = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|System")
	uint8 NumberOfThreads = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|System")
	uint8 MeshUpdatesPerFrame = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|System")
	FTimespan TileReleaseDelay = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|System")
	float TileSweepTime = 1.0f;
	/** Number of blocks along a zone's X axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Scale")
	int32 TileXUnits = 32;
	/** Number of blocks along a zone's Y axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Scale")
	int32 TileYUnits = 32;
	/** Size of a single block in world units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Scale")
	float UnitSize = 300.0f;
	/** Multiplier for heightmap*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "cashGen|Scale")
	float Amplitude = 5000.0f;
	/** Droplet erosion droplet amount *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Erosion")
	int32 DropletAmount = 0;
	/** Droplet erosion deposition rate *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Erosion")
	float DropletErosionMultiplier = 1.0f;
	/** Droplet erosion deposition rate *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Erosion")
	float DropletDespositionMultiplier = 1.0f;
	/** Droplet erosion deposition Theta *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Erosion")
	float DropletSedimentCapacity = 10.0f;
	/** Droplet erosion evaporation rate *EXPERIMENTAL* **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Erosion")
	float DropletEvaporationRate = 0.1f;
	/** Erosion floor cutoff **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Erosion")
	float DropletErosionFloor = 0.0f;

	/** Material for the terrain mesh */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Rendering")
	//UMaterial* TerrainMaterial;
	/** Material for the water mesh (will be instanced)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Rendering")
	UMaterialInstance* WaterMaterialInstance = nullptr;
	/** Cast Shadows */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Rendering")
	bool CastShadows = false;
	/* Generate a texture including heightmap and other information */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Rendering")
	bool GenerateSplatMap = false;
	/** If checked and numLODs > 1, material will be instanced and TerrainOpacity parameters used to dither LOD transitions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Rendering")
	bool DitheringLODTransitions = false;
	/** If no TerrainMaterial and LOD transitions disabled, just use the same static instance for all LODs **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Rendering")
	UMaterialInstance* TerrainMaterialInstance = nullptr;
	/** Make a dynamic material instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Water")
	bool MakeDynamicMaterialInstance = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Water")
	/** If checked, will use a single instanced mesh for water, otherwise a procmesh section with dynamic texture will be used */
	bool UseInstancedWaterMesh = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Water")
	/** If checked, will use a single instanced mesh for water, otherwise a procmesh section with dynamic texture will be used */
	UStaticMesh* WaterMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|Water")
	TEnumAsByte<ECollisionEnabled::Type> WaterCollision = ECollisionEnabled::Type::QueryAndPhysics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen|LODs")
	TArray<FCGLODConfig> LODs;

	FVector TileOffset = FVector::ZeroVector;
};
