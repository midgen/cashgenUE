

#include "UnrealLibNoise.h"
#include "../Exception.h"
#include "Power.h"

///Constructor
UPower::UPower(const FObjectInitializer& ObjectInit) : Super(ObjectInit) {}

float UPower::GetValue(FVector Coordinates)
{
	if(GetSourceModule(0) == NULL) throw ExceptionNoModule();
	if(GetSourceModule(1) == NULL) throw ExceptionNoModule();

	return FMath::Pow(GetSourceModule(0)->GetValue(Coordinates),
		GetSourceModule(1)->GetValue(Coordinates));
}
