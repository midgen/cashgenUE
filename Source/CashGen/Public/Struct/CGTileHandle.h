#pragma once
#include "CashGen.h"
#include "CGTileHandle.generated.h"

class ACGTile;

UENUM(BlueprintType)
enum class ETileStatus : uint8
{
	NOT_SPAWNED, SPAWNED, REQUESTED, TRANSITION, IDLE
};

/** Handle for tracking a single tile */
USTRUCT()
struct FCGTileHandle
{
	GENERATED_USTRUCT_BODY()
	// Current rendering status of the sector
	ETileStatus myStatus;
	uint8 myLOD;
	int32 myWaterISMIndex;
	// Handle to the tile actor
	UPROPERTY()
	ACGTile* myHandle;
	// Bitmask to indicate which players require this sector
	FDateTime myLastRequiredTimestamp;
};