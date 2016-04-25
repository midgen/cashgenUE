

#include "UnrealLibNoise.h"
#include "Voronoi.h"

const double SQRT_3 = 1.7320508075688772935;

UVoronoi::UVoronoi(const FObjectInitializer& ObjectInit) : Super(ObjectInit),
	Displacement(DEFAULT_VORONOI_DISPLACEMENT),
	ShouldEnableDistance(false),
	Frequency(DEFAULT_VORONOI_FREQUENCY),
	Seed(DEFAULT_VORONOI_SEED)
{
}

float UVoronoi::GetValue(FVector Coordinates)
{
	// This method could be more efficient by caching the seed values.  Fix
	// later.

	Coordinates.X *= Frequency;
	Coordinates.Y *= Frequency;
	Coordinates.Z *= Frequency;

	int xInt = (Coordinates.X > 0.0 ? (int)Coordinates.X : (int)Coordinates.X - 1);
	int yInt = (Coordinates.Y > 0.0 ? (int)Coordinates.Y : (int)Coordinates.Y - 1);
	int zInt = (Coordinates.Z > 0.0 ? (int)Coordinates.Z : (int)Coordinates.Z - 1);

	float minDist = 2147483647.0;
	float xCandidate = 0;
	float yCandidate = 0;
	float zCandidate = 0;

	// Inside each unit cube, there is a seed point at a random position.  Go
	// through each of the nearby cubes until we find a cube with a seed point
	// that is closest to the specified position.
	for (int zCur = zInt - 2; zCur <= zInt + 2; zCur++) {
		for (int yCur = yInt - 2; yCur <= yInt + 2; yCur++) {
			for (int xCur = xInt - 2; xCur <= xInt + 2; xCur++) {

				// Calculate the position and distance to the seed point inside of
				// this unit cube.
				float xPos = xCur + UNoiseGen::ValueNoise3D(xCur, yCur, zCur, Seed);
				float yPos = yCur + UNoiseGen::ValueNoise3D(xCur, yCur, zCur, Seed + 1);
				float zPos = zCur + UNoiseGen::ValueNoise3D(xCur, yCur, zCur, Seed + 2);
				float xDist = xPos - Coordinates.X;
				float yDist = yPos - Coordinates.Y;
				float zDist = zPos - Coordinates.Z;
				float dist = xDist * xDist + yDist * yDist + zDist * zDist;

				if (dist < minDist) {
					// This seed point is closer to any others found so far, so record
					// this seed point.
					minDist = dist;
					xCandidate = xPos;
					yCandidate = yPos;
					zCandidate = zPos;
				}
			}
		}
	}

	float value;
	if (ShouldEnableDistance) {
		// Determine the distance to the nearest seed point.
		float xDist = xCandidate - Coordinates.X;
		float yDist = yCandidate - Coordinates.Y;
		float zDist = zCandidate - Coordinates.Z;
		value = (FMath::Sqrt(xDist * xDist + yDist * yDist + zDist * zDist)
			) * SQRT_3 - 1.0;
	}
	else {
		value = 0.0;
	}

	// Return the calculated distance with the displacement value applied.
	return value + (Displacement * (float)UNoiseGen::ValueNoise3D(
		(int)(FMath::FloorToFloat(xCandidate)),
		(int)(FMath::FloorToFloat(yCandidate)),
		(int)(FMath::FloorToFloat(zCandidate))));
}


