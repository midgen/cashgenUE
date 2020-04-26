#pragma once

#include "CashGen/Public/CGTerrainManager.h"

#include <Runtime/Engine/Classes/Components/ActorComponent.h>

#include "CGTerrainTrackerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CASHGEN_API UCGTerrainTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

	bool isSetup = false;
public:	
	// Sets default values for this component's properties
	UCGTerrainTrackerComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	/* Sets actor invisible until inital terrain generation is complete */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cashgen")
	bool HideActorUntilTerrainComplete = false;

	/* Attempts to disable gravity on character until terrain generation is complete */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cashgen")
	bool DisableCharacterGravityUntilComplete = false;

	/* Attempts to teleport character to terrain surface when terrain generation is complete */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cashgen")
	bool TeleportToSurfaceOnTerrainComplete  = false;

	void OnTerrainComplete();

	FVector mySpawnLocation;
	ACGTerrainManager* MyTerrainManager;

	/* Attempts to teleport character to terrain surface when terrain generation is complete */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cashgen")
	int32 SpawnRayCastsPerFrame = 10;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnUnregister() override;

	bool isTerrainComplete = false;
	bool isSpawnPointFound = false;

};
