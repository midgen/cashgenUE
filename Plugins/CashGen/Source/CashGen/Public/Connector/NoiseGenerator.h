#pragma once
#include "cashgen.h"
#include "NoiseGenerator.generated.h"

UINTERFACE(Blueprintable)
class UNoiseGenerator : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


class INoiseGenerator
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual float GetValue(const float aX, const float aY, const float aZ) = 0;
};