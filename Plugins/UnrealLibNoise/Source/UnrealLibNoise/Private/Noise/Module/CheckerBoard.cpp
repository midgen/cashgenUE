

#include "UnrealLibNoise.h"
#include "../Noise.h"
#include "CheckerBoard.h"

/// Constructor.
UCheckerBoard::UCheckerBoard(const FObjectInitializer& ObjectInit) : Super(ObjectInit) {}

float UCheckerBoard::GetValue(FVector Coordinates)
{
	int ix = FMath::FloorToInt(UNoiseGen::MakeInt32Range(Coordinates.X));
	int iy = FMath::FloorToInt(UNoiseGen::MakeInt32Range(Coordinates.Y));
	int iz = FMath::FloorToInt(UNoiseGen::MakeInt32Range(Coordinates.Z));
	return (ix & 1 ^ iy & 1 ^ iz & 1) ? -1.0 : 1.0;
}



