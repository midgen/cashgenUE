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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cashgen")
	bool HideActorUntilTerrainComplete;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cashgen")
	bool DisableCharacterGravityUntilComplete;

	//FTerrainCompleteEvent OnTerrainCompleteEvent;

	void OnTerrainComplete();

	FVector mySpawnLocation;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
