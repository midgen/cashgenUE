#pragma once

#include "UFNNoiseGenerator.h"
#include "UFNScaleBiasModule.generated.h"

UCLASS()
class UNREALFASTNOISEPLUGIN_API UUFNScaleBiasModule : public UUFNNoiseGenerator
{
	GENERATED_UCLASS_BODY()
public:

	float GetNoise3D(float aX, float aY, float aZ) override;
	float GetNoise2D(float aX, float aY) override;
	
	UPROPERTY()
	UUFNNoiseGenerator* inputModule;

	float scale;
	float bias;

};