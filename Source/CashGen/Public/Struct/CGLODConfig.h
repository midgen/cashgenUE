
#pragma once
#include "CGLODConfig.generated.h"

USTRUCT(BlueprintType)
struct FCGLODConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ResolutionDivisor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isCollisionEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isTesselationEnabled;

};