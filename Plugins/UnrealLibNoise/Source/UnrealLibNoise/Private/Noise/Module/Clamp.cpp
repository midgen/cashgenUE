

#include "UnrealLibNoise.h"
#include "Clamp.h"

/// Constructor.
///
/// The default lower bound of the clamping range is set to
/// DEFAULT_CLAMP_LOWER_BOUND.
///
/// The default upper bound of the clamping range is set to
/// noise::module::DEFAULT_CLAMP_UPPER_BOUND.
UClamp::UClamp(const FObjectInitializer& ObjectInit) : Super(ObjectInit)
{
	LowerBound = DEFAULT_CLAMP_LOWER_BOUND;
	UpperBound = DEFAULT_CLAMP_UPPER_BOUND;
}

float UClamp::GetValue(FVector Coordinates)
{

	double value = GetSourceModule(0)->GetValue(Coordinates);
	if (value < LowerBound) {
		return LowerBound;
	}
	else if (value > UpperBound) {
		return UpperBound;
	}
	else {
		return value;
	}
}

void UClamp::SetBounds(float lowerBound, float upperBound)
{
	if (lowerBound > upperBound) throw new ExceptionInvalidParam;

	LowerBound = lowerBound;
	UpperBound = upperBound;
}


