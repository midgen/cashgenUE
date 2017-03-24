#pragma once
// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
#include "FastNoise/FastNoise.h"
#include "UFNBlueprintFunctionLibrary.generated.h"

UCLASS()
class UNREALFASTNOISEPLUGIN_API UUFNBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	// Creates a new noise generator module. Note that not all parameters may be relevant e.g. Fractal noise types will ignore Cellular parameters
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateNoiseGenerator(UObject* outer, ENoiseType noiseType, ECellularDistanceFunction cellularDistanceFunction, ECellularReturnType cellularReturnType, EFractalType fractalType, EInterp interpolation, int32 seed = 1337, int32 octaves = 4, float frequency = 0.001f, float lacunarity = 2.0f, float fractalGain = 0.5f);
	// Creates a Select module. Returns a value either from input1 or input 2, depending on the value returned from the select module. Has sine in/out smooth falloff option (may be wonky)
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateSelectModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* selectModule, ESelectInterpType interpolationType = ESelectInterpType::None, float falloff = 0.0f, float threshold = 0.0f, int32 steps = 4);
	// Creates Blend modules. Returns a blended value from input1 and input 2, based on the value returned from the select module. Blend range is from -1.0 to 1.0;
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateBlendModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* selectModule, UCurveFloat* blendCurve = nullptr);
	// Creates a Scale/Bias modules. Applies a multiplier, and or additive value to the value returned from the input
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateScaleBiasModule(UObject* outer, UUFNNoiseGenerator* inputModule, float scale = 1.0f, float bias = 0.0f);
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateWarpModule(UObject* outer, UUFNNoiseGenerator* inputModule, UUFNNoiseGenerator* warpModule, float multiplier, EWarpIterations warpIterations);
	// Creates an Add module. Adds two modules together, clamping the result and optionally accepting a third module as a mask with threshold
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateAddModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* maskModule = nullptr, float threshold = 1.0f);
	// Creates a Constant modules. Returns a constant value
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateConstantModule(UObject* outer, float constantValue);
	// Creates a simple (non-fractal) noise module
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateSimpleNoiseGenerator(UObject* outer, ESimpleNoiseType noiseType, int32 seed = 12345, float frequency = 0.1f, EInterp interpolation = EInterp::InterpLinear);
	// Creates a Fractal noise module
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateFractalNoiseGenerator(UObject* outer, EFractalNoiseType noiseType, int32 seed = 12345, float frequency = 0.1f, float fractalGain = 0.5f, EInterp interpolation = EInterp::InterpLinear, EFractalType fractalType = EFractalType::Billow, int32 octaves = 4, float lacunarity = 2.0f);
	// Creates a Cellular (Voronoi) noise module
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateCellularNoiseGenerator(UObject* outer, int32 seed = 12345, float frequency = 0.1f, ECellularDistanceFunction cellularDistanceFunction = ECellularDistanceFunction::Euclidean, ECellularReturnType cellularReturnType = ECellularReturnType::CellValue);
	// Creates a spline module.
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* CreateSplineGenerator(UObject* outer, float MaxDistance, float MinDistance, TArray<class USplineComponent*> Splines, UCurveFloat* falloffCurve = nullptr);
	// Creates a Select module. Returns a value either from input1 or input 2 or input 3, depending on the value returned from the select module.
	UFUNCTION(BlueprintPure, Category = "UnrealFastNoise")
	static UUFNNoiseGenerator* Create3SelectModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* inputModule3, UUFNNoiseGenerator* selectModule, float lowerThreshold = 0.0f, float upperThreshold = 0.0f, ESelectInterpType interpolationType = ESelectInterpType::None, float falloff = 0.0f, int32 steps = 4);

};