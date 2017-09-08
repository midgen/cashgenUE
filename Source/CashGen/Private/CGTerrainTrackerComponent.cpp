// Fill out your copyright notice in the Description page of Project Settings.

#include "CashGen.h"
#include "CGTerrainManager.h"
#include "GameFramework/Character.h"
#include "CGTerrainTrackerComponent.h"



// Sets default values for this component's properties
UCGTerrainTrackerComponent::UCGTerrainTrackerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UCGTerrainTrackerComponent::OnTerrainComplete()
{
	if (HideActorUntilTerrainComplete)
	{
		GetOwner()->SetActorHiddenInGame(false);
	}

	if (DisableCharacterGravityUntilComplete)
	{
		ACharacter* character = Cast<ACharacter>(GetOwner());
		if (character)
		{
			character->GetCharacterMovement()->GravityScale = 1.0f;
		}
	}
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
				thisTM->AddActorToTrack(GetOwner());
				MyTerrainManager = thisTM;
				isSetup = true;
				MyTerrainManager->OnTerrainComplete().AddUObject(this, &UCGTerrainTrackerComponent::OnTerrainComplete);
				if (HideActorUntilTerrainComplete)
				{
					GetOwner()->SetActorHiddenInGame(true);
				}

				if (DisableCharacterGravityUntilComplete)
				{
					ACharacter* character = Cast<ACharacter>(GetOwner());
					if (character)
					{
						character->GetCharacterMovement()->GravityScale = 0.0f;
					}
				}
				mySpawnLocation = GetOwner()->GetActorLocation();
				
				
			}
			break;

		}
		
	}
}

