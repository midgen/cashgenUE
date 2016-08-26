#include "UnrealFastNoisePlugin.h"
#include "FastNoise/FastNoise.h"
#include "UFNSelectModule.h"
#include "UFNBlendModule.h"
#include "UFNScaleBiasModule.h"
#include "UFNBlueprintFunctionLibrary.h"

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateNoiseGenerator(UObject* outer, ENoiseType noiseType, ECellularDistanceFunction cellularDistanceFunction, ECellularReturnType cellularReturnType , EFractalType fractalType, EInterp interpolation, int32 seed, int32 octaves, float frequency, float lacunarity)
{
	UFastNoise* noiseGen = NewObject<UFastNoise>(outer, UFastNoise::StaticClass());

	noiseGen->SetNoiseType(noiseType);
	noiseGen->SetSeed(seed);
	noiseGen->SetFractalOctaves(octaves);
	noiseGen->SetFrequency(frequency);
	noiseGen->SetFractalType(fractalType);
	noiseGen->SetFractalLacunarity(lacunarity);
	noiseGen->SetCellularDistanceFunction(cellularDistanceFunction);
	noiseGen->SetCellularReturnType(cellularReturnType);
	noiseGen->SetInterp(interpolation);

	return noiseGen;
}



UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateSelectModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* selectModule, float falloff, float threshold)
{
	if (!(inputModule1 && inputModule2 && selectModule && outer)){
		return nullptr;
	}

	UUFNSelectModule* newSelectModule = NewObject<UUFNSelectModule>(outer, UUFNSelectModule::StaticClass());

	newSelectModule->inputModule1 = inputModule1;
	newSelectModule->inputModule2 = inputModule2;
	newSelectModule->selectModule = selectModule;
	newSelectModule->falloff = falloff;
	newSelectModule->threshold = threshold;

	return newSelectModule;
}

UUFNNoiseGenerator* UUFNBlueprintFunctionLibrary::CreateBlendModule(UObject* outer, UUFNNoiseGenerator* inputModule1, UUFNNoiseGenerator* inputModule2, UUFNNoiseGenerator* selectModule)
{
	if (!(inputModule1 && inputModule2 && selectModule && outer)) {
		return nullptr;
	}

	UUFNBlendModule* blendModule = NewObject<UUFNBlendModule>(outer, UUFNBlendModule::StaticClass());

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

	UUFNScaleBiasModule* scaleBiasModule = NewObject<UUFNScaleBiasModule>(outer, UUFNScaleBiasModule::StaticClass());

	scaleBiasModule->inputModule = inputModule;
	scaleBiasModule->scale = scale;
	scaleBiasModule->bias = bias;

	return scaleBiasModule;
}

UUFNBlueprintFunctionLibrary::UUFNBlueprintFunctionLibrary(const class FObjectInitializer& obj)
	: Super(obj)
{

}