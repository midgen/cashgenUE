#pragma once

#include "UFNNoiseGenerator.h"
#include "UFN3SelectModule.generated.h"

UCLASS()
class UNREALFASTNOISEPLUGIN_API UUFN3SelectModule : public UUFNNoiseGenerator
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
		UUFNNoiseGenerator* inputModule3;
	UPROPERTY()
		UUFNNoiseGenerator* selectModule;

	float upperThreshold;
	float lowerThreshold;

	float falloff;
	ESelectInterpType interpType;
	int32 numSteps;

};