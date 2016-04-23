#pragma once
#include "cashgenUE.h"
#include "FoliageConfig.generated.h"

USTRUCT()
struct FFoliageConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Config Struct")
		UStaticMesh* TreeMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Config Struct")
		int32 NumInstancesPerBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Foliage Config Struct")
		TEnumAsByte<ECollisionResponse> PawnCollisionResponse;
};
