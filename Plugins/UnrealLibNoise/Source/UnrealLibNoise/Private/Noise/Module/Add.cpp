

#include "UnrealLibNoise.h"
#include "Add.h"

///Constructor
UAdd::UAdd(const FObjectInitializer& ObjectInit) : Super(ObjectInit) {}

float UAdd::GetValue(FVector Coordinates)
{
	return GetSourceModule(0)->GetValue(Coordinates) + GetSourceModule(1)->GetValue(Coordinates);
}


