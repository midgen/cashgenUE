

#include "UnrealLibNoise.h"
#include "Min.h"

///Constructor
UMin::UMin(const FObjectInitializer& ObjectInit) : Super(ObjectInit) {}

float UMin::GetValue(FVector Coordinates)
{
	if (GetSourceModule(0) == NULL) throw ExceptionNoModule();
	if (GetSourceModule(1) == NULL) throw ExceptionNoModule();

	double v0 = GetSourceModule(0)->GetValue(Coordinates);
	double v1 = GetSourceModule(1)->GetValue(Coordinates);
	return FMath::Min(v0, v1);
}


