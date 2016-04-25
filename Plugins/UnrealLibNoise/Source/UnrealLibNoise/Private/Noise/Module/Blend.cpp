

#include "UnrealLibNoise.h"
#include "Blend.h"

/// Constructor.
UBlend::UBlend(const FObjectInitializer& ObjectInit) : Super(ObjectInit) {}

float UBlend::GetValue(FVector Coordinates)
{
	if (GetSourceModule(0) == NULL || GetSourceModule(1) == NULL || GetSourceModule(2) == NULL) {
		throw ExceptionNoModule();
	}

	float v0 = GetSourceModule(0)->GetValue(Coordinates);
	float v1 = GetSourceModule(1)->GetValue(Coordinates);
	float alpha = (GetSourceModule(2)->GetValue(Coordinates) + 1.0) / 2.0;
	return UNoiseInterp::LinearInterp(v0, v1, alpha);
}