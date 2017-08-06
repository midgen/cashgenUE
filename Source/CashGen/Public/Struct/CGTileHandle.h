#pragma once
#include "cashgen.h"
#include "CGTileHandle.generated.h"

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
	// Handle to the tile actor
	ACGTile* myHandle;
	// Bitmask to indicate which players require this sector
	uint32 myPlayerMask;
};