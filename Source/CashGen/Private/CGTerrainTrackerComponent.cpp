// Fill out your copyright notice in the Description page of Project Settings.

#include "CashGen.h"
#include "CGTerrainTrackerComponent.h"


// Sets default values for this component's properties
UCGTerrainTrackerComponent::UCGTerrainTrackerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCGTerrainTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCGTerrainTrackerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!MyTerrainManager && !isSetup)
	{
		TArray<AActor*> results;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACGTerrainManager::StaticClass(), results);

		//Should only be one
		for (auto& result : results)
		{
			ACGTerrainManager* thisTM = Cast<ACGTerrainManager>(result);
			if (thisTM && thisTM->isReady)
			{
				thisTM->AddPawn(GetOwner());
				MyTerrainManager = thisTM;
				isSetup = true;
			}
			break;

		}
		
	}
}

