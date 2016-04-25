

#include "UnrealLibNoise.h"
#include "../Exception.h"
#include "Invert.h"

///Constructor
UInvert::UInvert(const FObjectInitializer& ObjectInit) : Super(ObjectInit) {}

float UInvert::GetValue(FVector Coordinates)
{
	if (GetSourceModule(0) == NULL) throw ExceptionNoModule();

	return -(GetSourceModule(0)->GetValue(Coordinates));
}


