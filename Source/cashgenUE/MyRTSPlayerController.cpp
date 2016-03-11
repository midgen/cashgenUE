// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenue.h"
#include "MyRTSPlayerController.h"

AMyRTSPlayerController::AMyRTSPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

	EnableInput(this);
}

void AMyRTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);
	InputComponent->BindAction("MouseClick", IE_Released, this, &AMyRTSPlayerController::HandleMouseClick);

}

void AMyRTSPlayerController::HandleMouseClick()
{
	FCollisionQueryParams MyTraceParams = FCollisionQueryParams(FName(TEXT("MyTrace")), true, this);
	MyTraceParams.bTraceComplex = true;
	MyTraceParams.bTraceAsyncScene = true;
	MyTraceParams.bReturnPhysicalMaterial = false;
	

	
	FCollisionResponseParams MyResponseParams = FCollisionResponseParams();

	FHitResult MyHitResult(ForceInit);

	FVector MyCastStart;
	FVector MyCastDirection;

	DeprojectMousePositionToWorld(MyCastStart, MyCastDirection);

	if (GetWorld()->LineTraceSingleByChannel(MyHitResult, MyCastStart, MyCastStart + (MyCastDirection * 100000.0f), ECollisionChannel::ECC_WorldStatic, MyTraceParams, MyResponseParams))
	{
		AActor* hitActor = MyHitResult.GetActor();
		if (hitActor)
		{
			GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Black, hitActor->GetActorLocation().ToString());
			

		}
	}
}