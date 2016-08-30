
#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "UFNConstantModule.h"

UUFNConstantModule::UUFNConstantModule(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UUFNConstantModule::GetNoise3D(float aX, float aY, float aZ)
{
	return constantValue;
}

float UUFNConstantModule::GetNoise2D(float aX, float aY)
{
	return constantValue;
}

