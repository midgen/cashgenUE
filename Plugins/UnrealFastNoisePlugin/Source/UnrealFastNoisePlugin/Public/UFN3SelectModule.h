#pragma once

#include "UFNNoiseGenerator.h"
#include "UFN3SelectModule.generated.h"

UCLASS()
class UUFN3SelectModule : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:

	virtual float GetNoise3D(float aX, float aY, float aZ) override;
	virtual float GetNoise2D(float aX, float aY) override;

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

private:
	float GetInterp2D(float aX, float aY, UUFNNoiseGenerator* input1, UUFNNoiseGenerator* input2, ESelectInterpType interpType, float alpha);
	float GetInterp3D(float aX, float aY, float aZ, UUFNNoiseGenerator* input1, UUFNNoiseGenerator* input2, ESelectInterpType interpType, float alpha);
};