#pragma once
// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
#include "FastNoise/FastNoise.h"
#include "UFNBlueprintFunctionLibrary.generated.h"

UCLASS()
class UUFNBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	// Creates a new noise generator module. Note that not all parameters may be relevant e.g. Fractal noise types will ignore Cellular parameters
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateNoiseGenerator(UObject* outer, ENoiseType noiseType, ECellularDistanceFunction cellularDistanceFunction, ECellularReturnType cellularReturnType, EFractalType fractalType, EInterp interpolation, int32 seed, int32 octaves, float frequency, float lacunarity);
	// Creates a Select module. Returns a value either from input1 or input 2, depending on the value returned from the select module. Has sine in/out smooth falloff option (may be wonky)
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateSelectModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* selectModule, float falloff, float threshold);
	// Creates Blend modules. Returns a blended value from input1 and input 2, based on the value returned from the select module. Blend range is from -1.0 to 1.0;
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateBlendModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* selectModule);
	// Creates a Scale/Bias modules. Applies a multiplier, and or additive value to the value returned from the input
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateScaleBiasModule(UObject* outer, UUFNNoiseGenerator* inputModule, float scale, float bias);
};