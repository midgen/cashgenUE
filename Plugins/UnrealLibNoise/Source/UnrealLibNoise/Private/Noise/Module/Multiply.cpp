

#include "UnrealLibNoise.h"
#include "../Exception.h"
#include "Multiply.h"

///Constuctor
UMultiply::UMultiply(const FObjectInitializer& ObjectInit) : Super(ObjectInit) {}

float UMultiply::GetValue(FVector Coordinates)
{
	if(GetSourceModule(0) == NULL) throw ExceptionNoModule();
	if(GetSourceModule(1) == NULL)  throw ExceptionNoModule();

	return GetSourceModule(0)->GetValue(Coordinates) *
		GetSourceModule(1)->GetValue(Coordinates);
}