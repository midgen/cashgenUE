#pragma once
#include "cashgen.h"
#include "UnrealLibNoiseConnector.h"
#include "NoiseModule.h"


float UUnrealLibNoiseConnector::GetValue(const float aX, const float aY, const float aZ)
{
	if (noiseModule != nullptr)
	{
		return noiseModule->GetValue(FVector(aX, aY, aZ));
	}
	else
	{
		return -1.0f;
	}
}


bool UUnrealLibNoiseConnector::BindGenerator(UNoiseModule* aNoiseModule)
{
	noiseModule = aNoiseModule;
	return true;
}