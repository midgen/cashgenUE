#pragma once
#include "cashgenUE.h"
#include "MeshConfig.generated.h"

/** Defines the spawning parameters for a single instanced static mesh */
USTRUCT()
struct FMeshConfig
{
	GENERATED_USTRUCT_BODY()
	/** The static mesh that will be spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Config Struct")
		UStaticMesh* Mesh;
	/** The number of instances that will be spawned per block*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Config Struct")
		float DensityMultiplier;
	/** How the instances will react to Pawn collisions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh Config Struct")
		TEnumAsByte<ECollisionResponse> PawnCollisionResponse;
	/** Will the mesh be aligned to the floor, or remain vertical in world space */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Mesh Config Struct")
		bool AlignWithPlane;
	/** Mininum value of random size */
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Mesh Config Struct")
		float ScaleMin;
	/** Maximum value of randome size*/
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Mesh Config Struct")
		float ScaleMax;
};
