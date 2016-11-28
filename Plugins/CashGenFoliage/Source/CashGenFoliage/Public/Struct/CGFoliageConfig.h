#pragma once
#include "cashgenfoliage.h"
#include "CGFoliageConfig.generated.h"

USTRUCT()
struct FCGFoliageConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Config Struct")
		UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Config Struct")
		int32 NumInstancesPerBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Config Struct")
		TEnumAsByte<ECollisionResponse> PawnCollisionResponse;
};
