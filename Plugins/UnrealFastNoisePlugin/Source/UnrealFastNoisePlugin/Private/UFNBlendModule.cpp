
#include "UnrealFastNoisePlugin.h"
#include "UFNNoiseGenerator.h"
#include "UFNBlendModule.h"

UUFNBlendModule::UUFNBlendModule(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

float UUFNBlendModule::GetNoise3D(float aX, float aY, float aZ)
{

	if (!(inputModule1 && inputModule2 && selectModule)) {
		return 0.0f;
	}

	float control = (selectModule->GetNoise3D(aX, aY, aZ) + 1.0f) / 2.0f;

	if (blendCurve)
	{
		control = blendCurve->GetFloatValue(control);
	}

	return FMath::Lerp(inputModule1->GetNoise3D(aX, aY, aZ), inputModule2->GetNoise3D(aX, aY, aZ), control);
}

float UUFNBlendModule::GetNoise2D(float aX, float aY)
{
	if (!(inputModule1 && inputModule2 && selectModule)) {
		return 0.0f;
	}

	float control = (selectModule->GetNoise2D(aX, aY) + 1.0f) / 2.0f;

	if (blendCurve)
	{
		control = blendCurve->GetFloatValue(control);
	}

	return FMath::Lerp(inputModule1->GetNoise2D(aX, aY), inputModule2->GetNoise2D(aX, aY), control);
}

