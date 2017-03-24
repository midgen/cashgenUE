#pragma once

#include "UFNNoiseGenerator.h"
#include "UFNConstantModule.generated.h"

UCLASS()
class UNREALFASTNOISEPLUGIN_API UUFNConstantModule : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:

	float GetNoise3D(float aX, float aY, float aZ) override;
	float GetNoise2D(float aX, float aY) override;

	float constantValue;

};