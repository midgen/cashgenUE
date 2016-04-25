

#include "UnrealLibNoise.h"
#include "Spheres.h"

/// Constructor.
///
/// The default frequency is set to
/// noise::module::DEFAULT_SPHERES_FREQUENCY.
USpheres::USpheres(const FObjectInitializer& ObjectInit) : Super(ObjectInit), Frequency(DEFAULT_SPHERES_FREQUENCY)
{
}

float USpheres::GetValue(FVector Coordinates)
{
	Coordinates.X *= Frequency;
	Coordinates.Y *= Frequency;
	Coordinates.Z *= Frequency;

	float distFromCenter = FMath::Sqrt(Coordinates.X * Coordinates.X + Coordinates.Y * Coordinates.Y + Coordinates.Z * Coordinates.Z);
	float distFromSmallerSphere = distFromCenter - FMath::FloorToFloat(distFromCenter);
	float distFromLargerSphere = 1.0 - distFromSmallerSphere;
	float nearestDist = FMath::Min(distFromSmallerSphere, distFromLargerSphere);
	return 1.0 - (nearestDist * 4.0); // Puts it in the -1.0 to +1.0 range.
}


