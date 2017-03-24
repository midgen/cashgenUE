#pragma once

#include "UFNNoiseGenerator.h"
#include "UFNAddModule.generated.h"

UCLASS()
class UUFNAddModule : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:

	float GetNoise3D(float aX, float aY, float aZ) override;
	float GetNoise2D(float aX, float aY) override;
	UPROPERTY()
	UUFNNoiseGenerator* inputModule1;
	UPROPERTY()
	UUFNNoiseGenerator* inputModule2;
	UPROPERTY()
	UUFNNoiseGenerator* maskModule;

	float threshold;
};