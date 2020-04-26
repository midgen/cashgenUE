#pragma once

#include "CGSettings.generated.h"

/**
* Implements the settings for the CashGen Plugin
*/
UCLASS(config = CashGen, BlueprintType)
class CASHGEN_API UCGSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = Debug)
	bool ShowTimings = false;
};
