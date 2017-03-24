#pragma once


#include "UFNNoiseGenerator.generated.h"

UCLASS(BlueprintType)
class UNREALFASTNOISEPLUGIN_API UUFNNoiseGenerator : public UObject
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category="UnrealFastNoise")
	virtual float GetNoise2D(float aX, float aY);
	UFUNCTION(BlueprintCallable, Category = "UnrealFastNoise")
	virtual float GetNoise3D(float aX, float aY, float aZ);
};