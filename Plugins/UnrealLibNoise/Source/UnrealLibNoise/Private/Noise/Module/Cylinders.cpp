

#include "UnrealLibNoise.h"
#include "Cylinders.h"

/// Constructor.
///
/// The default frequency is set to
/// DEFAULT_CYLINDERS_FREQUENCY.
UCylinders::UCylinders(const FObjectInitializer& ObjectInit) : Super(ObjectInit) { Frequency = DEFAULT_CYLINDERS_FREQUENCY; }

float UCylinders::GetValue(FVector Coordinates) 
{
	Coordinates.X *= Frequency;
	Coordinates.Z *= Frequency;

	float distFromCenter = FMath::Sqrt(Coordinates.X * Coordinates.X + Coordinates.Z * Coordinates.Z);
	float distFromSmallerSphere = distFromCenter - FMath::FloorToFloat(distFromCenter);
	float distFromLargerSphere = 1.0 - distFromSmallerSphere;
	float nearestDist = FMath::Min(distFromSmallerSphere, distFromLargerSphere);
	return 1.0 - (nearestDist * 4.0); // Puts it in the -1.0 to +1.0 range.
}


