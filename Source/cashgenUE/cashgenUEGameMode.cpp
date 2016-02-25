// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "cashgenUEGameMode.h"
#include "MyRTSPlayerController.h"
#include "MyRTSPawn.h"

AcashgenUEGameMode::AcashgenUEGameMode()
{

	PlayerControllerClass = AMyRTSPlayerController::StaticClass();
	DefaultPawnClass = AMyRTSPawn::StaticClass();
}
