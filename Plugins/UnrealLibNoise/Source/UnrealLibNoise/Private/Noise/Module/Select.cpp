

#include "UnrealLibNoise.h"
#include "../Noise.h"
#include "Select.h"

/// Constructor.
///
/// The default falloff value at the edge transition is set to
/// DEFAULT_SELECT_EDGE_FALLOFF.
///
/// The default lower bound of the selection range is set to
/// DEFAULT_SELECT_LOWER_BOUND.
///
/// The default upper bound of the selection range is set to
///DEFAULT_SELECT_UPPER_BOUND.
USelect::USelect(const FObjectInitializer& ObjectInit) : Super(ObjectInit) ,EdgeFalloff(DEFAULT_SELECT_EDGE_FALLOFF), LowerBound(DEFAULT_SELECT_LOWER_BOUND), UpperBound(DEFAULT_SELECT_UPPER_BOUND) 
{ 
	SourceMoudules.Empty();

	if (GetSourceModuleCount() > 0) {

		for (int i = 0; i < GetSourceModuleCount(); i++) {
			SourceMoudules.Add(NULL);
		}
	}
	else {
		SourceMoudules.Empty();
	}
}

float USelect::GetValue(FVector Coordinates)
{
	//if (IsValid(GetSourceModule(0))) throw ExceptionNoModule();
	//if (IsValid(GetSourceModule(1))) throw ExceptionNoModule();
	//if (IsValid(GetSourceModule(2))) throw ExceptionNoModule();

	float controlValue = GetSourceModule(2)->GetValue(Coordinates);
	float alpha;
	if (EdgeFalloff > 0.0) {
		if (controlValue < (LowerBound - EdgeFalloff)) {
			// The output value from the control module is below the selector
			// threshold; return the output value from the first source module.
			return GetSourceModule(0)->GetValue(Coordinates);

		}
		else if (controlValue < (LowerBound + EdgeFalloff)) {
			// The output value from the control module is near the lower end of the
			// selector threshold and within the smooth curve. Interpolate between
			// the output values from the first and second source modules.
			double lowerCurve = (LowerBound - EdgeFalloff);
			double upperCurve = (LowerBound + EdgeFalloff);
			alpha = UNoiseInterp::SCurve3(
				(controlValue - lowerCurve) / (upperCurve - lowerCurve));
			return UNoiseInterp::LinearInterp(GetSourceModule(0)->GetValue(Coordinates),
				GetSourceModule(1)->GetValue(Coordinates),
				alpha);

		}
		else if (controlValue < (UpperBound - EdgeFalloff)) {
			// The output value from the control module is within the selector
			// threshold; return the output value from the second source module.
			return GetSourceModule(1)->GetValue(Coordinates);

		}
		else if (controlValue < (UpperBound + EdgeFalloff)) {
			// The output value from the control module is near the upper end of the
			// selector threshold and within the smooth curve. Interpolate between
			// the output values from the first and second source modules.
			double lowerCurve = (UpperBound - EdgeFalloff);
			double upperCurve = (UpperBound + EdgeFalloff);
			alpha = UNoiseInterp::SCurve3(
				(controlValue - lowerCurve) / (upperCurve - lowerCurve));
			return UNoiseInterp::LinearInterp(GetSourceModule(1)->GetValue(Coordinates),
				GetSourceModule(0)->GetValue(Coordinates),
				alpha);

		}
		else {
			// Output value from the control module is above the selector threshold;
			// return the output value from the first source module.
			return GetSourceModule(0)->GetValue(Coordinates);
		}
	}
	else {
		if (controlValue < LowerBound || controlValue > UpperBound) {
			return GetSourceModule(0)->GetValue(Coordinates);
		}
		else {
			return GetSourceModule(1)->GetValue(Coordinates);
		}
	}
}

void USelect::SetBounds(float lowerBound, float upperBound)
{
	if(lowerBound > upperBound) throw ExceptionNoModule();

	LowerBound = lowerBound;
	UpperBound = upperBound;

	// Make sure that the edge falloff curves do not overlap.
	SetEdgeFalloff(EdgeFalloff);
}

void USelect::SetEdgeFalloff(float edgeFalloff)
{
	// Make sure that the edge falloff curves do not overlap.
	float boundSize = UpperBound - LowerBound;
	EdgeFalloff = (edgeFalloff > boundSize / 2) ? boundSize / 2 : edgeFalloff;
}


