
#pragma once
#include "CGLODConfig.generated.h"

USTRUCT(BlueprintType)
struct FCGLODConfig
{
	GENERATED_USTRUCT_BODY()
	/** Radius in sectors to spawn terrain around an actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SectorRadius;
	/** Factor to reduce sector mesh resolution by from base value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ResolutionDivisor;
	/** Cook collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isCollisionEnabled;
	/** Calculate tesselation indices */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isTesselationEnabled;

};