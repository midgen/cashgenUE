// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CGTerrainManager.h"
#include "CGTerrainTrackerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CASHGEN_API UCGTerrainTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

	bool isSetup = false;
public:	
	// Sets default values for this component's properties
	UCGTerrainTrackerComponent();

	ACGTerrainManager* MyTerrainManager;

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



	/* Attempts to teleport character to terrain surface when terrain generation is complete */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cashgen")
	int32 SpawnRayCastsPerFrame = 10;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnUnregister() override;

	bool isTerrainComplete = false;
	bool isSpawnPointFound = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	
};
