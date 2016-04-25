

#include "UnrealLibNoise.h"
#include "../Exception.h"
#include "Exponent.h"

/// Constructor.
///
/// The default exponent is set to noise::module::DEFAULT_EXPONENT.
UExponent::UExponent(const FObjectInitializer& ObjectInit) : Super(ObjectInit), Exponent(DEFAULT_EXPONENT)
{
}

float UExponent::GetValue(FVector Coordinates)
{
	if (GetSourceModule(0) == NULL) throw ExceptionNoModule();

	float value = GetSourceModule(0)->GetValue(Coordinates);
	return (FMath::Pow(FMath::Abs((value + 1.0) / 2.0), Exponent) * 2.0 - 1.0);
}

