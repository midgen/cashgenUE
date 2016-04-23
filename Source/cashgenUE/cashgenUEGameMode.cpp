// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "cashgenUEGameMode.h"
#include "FlyingThiefPawn.h"

AcashgenUEGameMode::AcashgenUEGameMode()
{
	DefaultPawnClass = AFlyingThiefPawn::StaticClass();
}
