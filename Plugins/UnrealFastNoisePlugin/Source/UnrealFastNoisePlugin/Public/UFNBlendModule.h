#pragma once

#include "UFNNoiseGenerator.h"
#include "UFNBlendModule.generated.h"

UCLASS()
class UNREALFASTNOISEPLUGIN_API UUFNBlendModule : public UUFNNoiseGenerator
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
	UUFNNoiseGenerator* selectModule;

	float falloff;
	UPROPERTY()
	UCurveFloat* blendCurve;

};