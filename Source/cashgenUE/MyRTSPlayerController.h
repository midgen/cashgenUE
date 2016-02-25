#pragma once

#include "GameFramework/PlayerController.h"
#include "WorldManager.h"
#include "MyRTSPlayerController.generated.h"

UCLASS()
class CASHGENUE_API AMyRTSPlayerController : public APlayerController
{
	GENERATED_BODY()

	AMyRTSPlayerController();
	void SetupInputComponent();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen Manager")
	AWorldManager* worldManager;
};
