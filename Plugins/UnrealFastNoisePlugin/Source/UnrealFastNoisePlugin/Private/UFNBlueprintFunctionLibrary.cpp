#include "UnrealFastNoisePlugin.h"
#include "FastNoise/FastNoise.h"
#include "UFNSelectModule.h"
#include "UFN3SelectModule.h"
#include "UFNBlendModule.h"
#include "UFNScaleBiasModule.h"
#include "UFNConstantModule.h"
#include "UFNBlueprintFunctionLibrary.h"

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateNoiseGenerator(UObject* outer, ENoiseType noiseType, ECellularDistanceFunction cellularDistanceFunction, ECellularReturnType cellularReturnType , EFractalType fractalType, EInterp interpolation, int32 seed, int32 octaves, float frequency, float lacunarity, float fractalGain, EPositionWarpType positionWarpType, float positionWarpAmplitude)
{
	UFastNoise* noiseGen = NewObject<UFastNoise>(outer, FName("NoiseGen"));

	noiseGen->SetNoiseType(noiseType);
	noiseGen->SetSeed(seed);
	noiseGen->SetFractalOctaves(octaves);
	noiseGen->SetFrequency(frequency);
	noiseGen->SetFractalType(fractalType);
	noiseGen->SetFractalLacunarity(lacunarity);
	noiseGen->SetFractalGain(fractalGain);
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

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateFractalNoiseGenerator(UObject* outer, EFractalNoiseType noiseType, int32 seed, float frequency, float fractalGain, EInterp interpolation, EFractalType fractalType, int32 octaves, float lacunarity, EPositionWarpType positionWarpType, float positionWarpAmplitude)
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
	noiseGen->SetFractalGain(fractalGain);
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

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::Create3SelectModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* inputModule3, UUFNNoiseGenerator* selectModule, float lowerThreshold /*= 0.0f*/, float upperThreshold /*= 0.0f*/, ESelectInterpType interpolationType /*= ESelectInterpType::None*/, float falloff /*= 0.0f*/, int32 steps /*= 4*/)
{
	if (!(inputModule1 && inputModule2 && inputModule3 && selectModule && outer)) {
		return nullptr;
	}

	UUFN3SelectModule* newSelectModule = NewObject<UUFN3SelectModule>(outer, FName("Select"));

	newSelectModule->inputModule1 = inputModule1;
	newSelectModule->inputModule2 = inputModule2;
	newSelectModule->inputModule3 = inputModule3;
	newSelectModule->selectModule = selectModule;
	newSelectModule->falloff = falloff;
	newSelectModule->lowerThreshold = lowerThreshold;
	newSelectModule->upperThreshold = upperThreshold;
	newSelectModule->interpType = interpolationType;
	newSelectModule->numSteps = steps;

	return newSelectModule;
}

UUFNBlueprintFunctionLibrary::UUFNBlueprintFunctionLibrary(const class FObjectInitializer& obj)
	: Super(obj)
{

}