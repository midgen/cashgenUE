#include "UnrealFastNoisePlugin.h"
#include "FastNoise/FastNoise.h"
#include "UFNSelectModule.h"
#include "UFNBlendModule.h"
#include "UFNScaleBiasModule.h"
#include "UFNConstantModule.h"
#include "UFNBlueprintFunctionLibrary.h"

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateNoiseGenerator(UObject* outer, ENoiseType noiseType, ECellularDistanceFunction cellularDistanceFunction, ECellularReturnType cellularReturnType , EFractalType fractalType, EInterp interpolation, int32 seed, int32 octaves, float frequency, float lacunarity, EPositionWarpType positionWarpType, float positionWarpAmplitude)
{
	UFastNoise* noiseGen = NewObject<UFastNoise>(outer, FName("NoiseGen"));

	noiseGen->SetNoiseType(noiseType);
	noiseGen->SetSeed(seed);
	noiseGen->SetFractalOctaves(octaves);
	noiseGen->SetFrequency(frequency);
	noiseGen->SetFractalType(fractalType);
	noiseGen->SetFractalLacunarity(lacunarity);
	noiseGen->SetCellularDistanceFunction(cellularDistanceFunction);
	noiseGen->SetCellularReturnType(cellularReturnType);
	noiseGen->SetInterp(interpolation);
	noiseGen->SetPositionWarpAmp(positionWarpAmplitude);
	noiseGen->SetPositionWarpType(positionWarpType);

	return noiseGen;
}



UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateSelectModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* selectModule, ESelectInterpType interpolationType, float falloff, float threshold, int32 numSteps)
{
	if (!(inputModule1 && inputModule2 && selectModule && outer)){
		return nullptr;
	}

	UUFNSelectModule* newSelectModule = NewObject<UUFNSelectModule>(outer, FName("Select"));

	newSelectModule->inputModule1 = inputModule1;
	newSelectModule->inputModule2 = inputModule2;
	newSelectModule->selectModule = selectModule;
	newSelectModule->falloff = falloff;
	newSelectModule->threshold = threshold;
	newSelectModule->interpType = interpolationType;
	newSelectModule->numSteps = numSteps;

	return newSelectModule;
}

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateBlendModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* selectModule)
{
	if (!(inputModule1 && inputModule2 && selectModule && outer)) {
		return nullptr;
	}

	UUFNBlendModule* blendModule = NewObject<UUFNBlendModule>(outer, FName("Blend"));

	blendModule->inputModule1 = inputModule1;
	blendModule->inputModule2 = inputModule2;
	blendModule->selectModule = selectModule;

	return blendModule;
}

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateScaleBiasModule(UObject* outer, UUFNNoiseGenerator* inputModule, float scale, float bias)
{
	if (!(inputModule && outer)) {
		return nullptr;
	}

	UUFNScaleBiasModule* scaleBiasModule = NewObject<UUFNScaleBiasModule>(outer);

	scaleBiasModule->inputModule = inputModule;
	scaleBiasModule->scale = scale;
	scaleBiasModule->bias = bias;

	return scaleBiasModule;
}

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateConstantModule(UObject* outer, float constantValue)
{
	UUFNConstantModule* noiseGen = NewObject<UUFNConstantModule>(outer);

	noiseGen->constantValue = constantValue;

	return noiseGen;
}

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateSimpleNoiseGenerator(UObject* outer, ESimpleNoiseType noiseType, int32 seed, float frequency, EInterp interpolation , EPositionWarpType positionWarpType, float positionWarpAmplitude)
{
	UFastNoise* noiseGen = NewObject<UFastNoise>(outer);

	switch (noiseType)
	{
		case ESimpleNoiseType::SimpleGradient:
			noiseGen->SetNoiseType(ENoiseType::Gradient);
			break;
		case ESimpleNoiseType::SimpleSimplex:
			noiseGen->SetNoiseType(ENoiseType::Simplex);
			break;
		case ESimpleNoiseType::SimpleValue:
			noiseGen->SetNoiseType(ENoiseType::Value);
			break;
		case ESimpleNoiseType::SimpleWhiteNoise:
			noiseGen->SetNoiseType(ENoiseType::WhiteNoise);
			break;
	}

	noiseGen->SetSeed(seed);
	noiseGen->SetFrequency(frequency);
	noiseGen->SetInterp(interpolation);
	noiseGen->SetPositionWarpAmp(positionWarpAmplitude);
	noiseGen->SetPositionWarpType(positionWarpType);

	return noiseGen;
}

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateFractalNoiseGenerator(UObject* outer, EFractalNoiseType noiseType, int32 seed, float frequency, EInterp interpolation, EFractalType fractalType, int32 octaves, float lacunarity, EPositionWarpType positionWarpType, float positionWarpAmplitude)
{
	UFastNoise* noiseGen = NewObject<UFastNoise>(outer);

	switch (noiseType)
	{
	case EFractalNoiseType::FractalGradient:
		noiseGen->SetNoiseType(ENoiseType::GradientFractal);
		break;
	case EFractalNoiseType::FractalSimplex:
		noiseGen->SetNoiseType(ENoiseType::SimplexFractal);
		break;
	case EFractalNoiseType::FractalValue:
		noiseGen->SetNoiseType(ENoiseType::ValueFractal);
		break;
	}

	noiseGen->SetSeed(seed);
	noiseGen->SetFractalOctaves(octaves);
	noiseGen->SetFrequency(frequency);
	noiseGen->SetFractalType(fractalType);
	noiseGen->SetFractalLacunarity(lacunarity);
	noiseGen->SetInterp(interpolation);
	noiseGen->SetPositionWarpAmp(positionWarpAmplitude);
	noiseGen->SetPositionWarpType(positionWarpType);

	return noiseGen;
}

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateCellularNoiseGenerator(UObject* outer, int32 seed, float frequency, ECellularDistanceFunction cellularDistanceFunction, ECellularReturnType cellularReturnType, EPositionWarpType positionWarpType, float positionWarpAmplitude)
{
	UFastNoise* noiseGen = NewObject<UFastNoise>(outer);

	noiseGen->SetNoiseType(ENoiseType::Cellular);
	noiseGen->SetSeed(seed);
	noiseGen->SetFrequency(frequency);
	noiseGen->SetCellularDistanceFunction(cellularDistanceFunction);
	noiseGen->SetCellularReturnType(cellularReturnType);
	noiseGen->SetPositionWarpAmp(positionWarpAmplitude);
	noiseGen->SetPositionWarpType(positionWarpType);

	return noiseGen;
}

UUFNBlueprintFunctionLibrary::UUFNBlueprintFunctionLibrary(const class FObjectInitializer& obj)
	: Super(obj)
{

}