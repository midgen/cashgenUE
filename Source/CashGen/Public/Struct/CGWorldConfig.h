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
	/** Number of blocks along a zone's Y axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Main")
		int32 SubDivFaceLimit = 4;
	/** Multiplier for heightmap*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "cashGen | Main")
		float Amplitude = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "cashGen | Main")
		uint8 MeshUpdatesPerFrame = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "cashGen | Main")
		bool DebugSubdivisions = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cashgen | Main")
		int32 MeshDataPoolSize = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cashgen | Main")
		bool CollisionEnabled = false;
	/** Material for the terrain mesh */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen | Erosion")
		//UMaterial* TerrainMaterialInstanceParent;


};