// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "cashgenUE.h"
#include "TP_FlyingGameMode.h"
#include "TP_FlyingPawn.h"

ATP_FlyingGameMode::ATP_FlyingGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = ATP_FlyingPawn::StaticClass();
}
