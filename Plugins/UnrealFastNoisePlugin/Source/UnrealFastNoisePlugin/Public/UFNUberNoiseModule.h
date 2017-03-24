#pragma once

#include "UFNNoiseGenerator.h"
#include "UFNUberNoiseModule.generated.h"

UCLASS()
class UNREALFASTNOISEPLUGIN_API UUFNUberNoiseModule : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:

	float GetNoise3D(float aX, float aY, float aZ) override;
	float GetNoise2D(float aX, float aY) override;

	FVector GetNoise2DDerivative(float aX, float aY);

	UPROPERTY()
		UUFNNoiseGenerator* inputModule;

	float sampleRange;
	float iterations;

};