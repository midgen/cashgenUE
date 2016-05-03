#pragma once
#include "cashgen.h"
#include "NoiseGenerator.h"
#include "UnrealLibNoise.h"
#include "NoiseModule.h"
#include "UnrealLibNoiseConnector.generated.h"

UCLASS()
class UUnrealLibNoiseConnector : public UObject, public INoiseGenerator
{
	GENERATED_BODY()
private:
	UNoiseModule* noiseModule;
public:
		virtual float GetValue(const float aX, const float aY, const float aZ);
		UFUNCTION(BlueprintCallable, Category = "UnrealLibNoiseConnector")
		bool BindGenerator(UNoiseModule* aNoiseModule);
};