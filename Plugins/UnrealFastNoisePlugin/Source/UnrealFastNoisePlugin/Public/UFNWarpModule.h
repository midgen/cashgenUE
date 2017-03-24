#pragma once

#include "UFNNoiseGenerator.h"
#include "UFNWarpModule.generated.h"


UCLASS()
class UNREALFASTNOISEPLUGIN_API UUFNWarpModule : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:

	float GetNoise3D(float aX, float aY, float aZ) override;
	float GetNoise2D(float aX, float aY) override;

	UPROPERTY()
	UUFNNoiseGenerator* inputModule;

	UPROPERTY()
	UUFNNoiseGenerator* warpModule;

	float Iteration1XOffset;
	float Iteration1YOffset;
	float Iteration1ZOffset;

	float Iteration2XOffset1;
	float Iteration2YOffset1;
	float Iteration2ZOffset1;
	float Iteration2XOffset2;
	float Iteration2YOffset2;
	float Iteration2ZOffset2;

	EWarpIterations warpIterations;

	float unitSize;

	float multiplier;

};