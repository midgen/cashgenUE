#pragma once
#pragma once

#include "UFNNoiseGenerator.h"
#include "UFNBlendModule.generated.h"

UCLASS()
class UUFNBlendModule : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:

	virtual float GetNoise3D(float aX, float aY, float aZ) override;
	virtual float GetNoise2D(float aX, float aY) override;

	UUFNNoiseGenerator* inputModule1;
	UUFNNoiseGenerator* inputModule2;
	UUFNNoiseGenerator* selectModule;

	float falloff;

};