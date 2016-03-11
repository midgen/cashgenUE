// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "cashgenUEGameMode.h"
#include "MyRTSPlayerController.h"
#include "MyRTSPawn.h"
#include "FlyingThiefPawn.h"

AcashgenUEGameMode::AcashgenUEGameMode()
{
	PlayerControllerClass = AMyRTSPlayerController::StaticClass();
	DefaultPawnClass = AFlyingThiefPawn::StaticClass();
}
