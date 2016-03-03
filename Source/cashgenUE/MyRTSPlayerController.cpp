// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenue.h"
#include "MyRTSPlayerController.h"

AMyRTSPlayerController::AMyRTSPlayerController()
{
	//bShowMouseCursor = true;
	//bEnableClickEvents = true;
	//bEnableMouseOverEvents = false;
	//DefaultMouseCursor = EMouseCursor::Crosshairs;

	EnableInput(this);
}

void AMyRTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);
	//InputComponent->BindAction("MouseClick", IE_Released, this, &AMyRTSPlayerController::HandleMouseClick);

}
