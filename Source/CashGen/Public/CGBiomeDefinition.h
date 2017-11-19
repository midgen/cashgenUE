#pragma once
#include "CoreMinimal.h"
#include "CGBiomeDefinition.generated.h"

class UUFNNoiseGenerator;


UCLASS(Blueprintable, BlueprintType, meta = (ShowWorldContextPin))
class CASHGEN_API UCGBiomeDefinition : public UObject
{
	GENERATED_BODY()



public:
	UCGBiomeDefinition(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CashGen")
	UUFNNoiseGenerator* myHeightMapNoiseGenerator;



};