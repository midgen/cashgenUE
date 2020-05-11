#pragma once

#include "CGLODConfig.generated.h"

USTRUCT(BlueprintType)
struct FCGLODConfig
{
	GENERATED_BODY()

	FCGLODConfig()
		: SectorRadius(0)
		, ResolutionDivisor(0)
		, isCollisionEnabled(true)
	{
	}

	/** Radius in sectors to spawn terrain around an actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	int SectorRadius;
	/** Factor to reduce sector mesh resolution by from base value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	uint8 ResolutionDivisor;
	/** Cook collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen")
	bool isCollisionEnabled;
};