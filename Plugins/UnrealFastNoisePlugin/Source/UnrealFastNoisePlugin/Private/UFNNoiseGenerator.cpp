#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"

UUFNNoiseGenerator::UUFNNoiseGenerator(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

float UUFNNoiseGenerator::GetNoise2D(float aX, float aY)
{
	return -2.0f;
}

float UUFNNoiseGenerator::GetNoise3D(float aX, float aY, float aZ)
{
	return -2.0f;
}