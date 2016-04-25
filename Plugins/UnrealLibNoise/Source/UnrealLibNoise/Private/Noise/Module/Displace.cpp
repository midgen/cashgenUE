

#include "UnrealLibNoise.h"
#include "Displace.h"

///Constructor
UDisplace::UDisplace(const FObjectInitializer& ObjectInit) : Super(ObjectInit) {}

float UDisplace::GetValue(FVector Coordinates)
{
	if (GetSourceModule(0) == NULL) throw ExceptionNoModule();
	if (GetSourceModule(1) == NULL) throw ExceptionNoModule();
	if (GetSourceModule(2) == NULL) throw ExceptionNoModule();
	if (GetSourceModule(3) == NULL) throw ExceptionNoModule();

	// Get the output values from the three displacement modules.  Add each
	// value to the corresponding coordinate in the input value.
	double xDisplace = Coordinates.X + (GetSourceModule(1)->GetValue(Coordinates));
	double yDisplace = Coordinates.Y + (GetSourceModule(2)->GetValue(Coordinates));
	double zDisplace = Coordinates.Z + (GetSourceModule(3)->GetValue(Coordinates));

	// Retrieve the output value using the offsetted input value instead of
	// the original input value.
	return GetSourceModule(0)->GetValue(FVector(xDisplace, yDisplace, zDisplace));
}


