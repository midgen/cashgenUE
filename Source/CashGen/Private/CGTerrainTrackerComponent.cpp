// Fill out your copyright notice in the Description page of Project Settings.
#include "CGTerrainTrackerComponent.h"
#include "CashGen.h"
#include "CGTerrainManager.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"




// Sets default values for this component's properties
UCGTerrainTrackerComponent::UCGTerrainTrackerComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}


void UCGTerrainTrackerComponent::OnTerrainComplete()
{
	if (HideActorUntilTerrainComplete && !TeleportToSurfaceOnTerrainComplete)
	{
		GetOwner()->SetActorHiddenInGame(false);
	}

	if (DisableCharacterGravityUntilComplete && !TeleportToSurfaceOnTerrainComplete)
	{
		ACharacter* character = Cast<ACharacter>(GetOwner());
		if (character)
		{
			character->GetCharacterMovement()->GravityScale = 1.0f;
		}
	}

	isTerrainComplete = true;

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
				isSetup = true;
				thisTM->AddActorToTrack(GetOwner());
				MyTerrainManager = thisTM;
				
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
				if (TeleportToSurfaceOnTerrainComplete)
				{
					mySpawnLocation = GetOwner()->GetActorLocation();
				}
				
				
				
			}
			break;

		}
		
	}

	if (!isSpawnPointFound && isTerrainComplete && TeleportToSurfaceOnTerrainComplete)
	{
		int32 raycastsRemainingThisFrame = SpawnRayCastsPerFrame;

		while (raycastsRemainingThisFrame > 0)
		{


			FVector traceStart = mySpawnLocation + FVector(FMath::RandRange(-10000.0f, 10000.0f), FMath::RandRange(-100000.0f, 100000.0f), 5000.0f);
			FVector traceEnd = traceStart + FVector(0.f, 0.f, -50000.0f);
			FCollisionQueryParams traceParams;

			traceParams.bTraceComplex = true;
			traceParams.bReturnPhysicalMaterial = true;

			/*		const FName TraceTag("SpawnTraceTag");

					GetWorld()->DebugDrawTraceTag = TraceTag;

					traceParams.TraceTag = TraceTag;*/

			FHitResult hitResult;

			if (GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_GameTraceChannel1, traceParams)
				&& hitResult.Location.Z > 10.0f)
			{

				GetOwner()->SetActorLocation(hitResult.Location + FVector(0.0f, 0.0f, 10.0f));
				ACharacter* character = Cast<ACharacter>(GetOwner());
				if (character)
				{
					character->GetCharacterMovement()->GravityScale = 1.0f;
				}
				GetOwner()->SetActorHiddenInGame(false);
				isSpawnPointFound = true;
				break;
			}

			raycastsRemainingThisFrame--;
		}

	}



	
}

void UCGTerrainTrackerComponent::OnUnregister()
{
	if (MyTerrainManager)
	{
		MyTerrainManager->RemoveActorToTrack(GetOwner());
	}
	Super::OnUnregister();
}

