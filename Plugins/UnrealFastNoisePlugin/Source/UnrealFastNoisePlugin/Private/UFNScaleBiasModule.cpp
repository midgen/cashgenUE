
#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "UFNScaleBiasModule.h"

UUFNScaleBiasModule::UUFNScaleBiasModule(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UUFNScaleBiasModule::GetNoise3D(float aX, float aY, float aZ)
{
	if (!(inputModule)) {
		return 0.0f;
	}

	return (inputModule->GetNoise3D(aX, aY, aZ) * scale) + bias;
}

float UUFNScaleBiasModule::GetNoise2D(float aX, float aY)
{
	if (!(inputModule)) {
		return 0.0f;
	}

	return (inputModule->GetNoise2D(aX, aY) * scale) + bias;
}

