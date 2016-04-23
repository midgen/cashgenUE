#pragma once
#include "cashgenUE.h"
#include "ZoneJob.generated.h"

/** Defines a single mesh generation job */
USTRUCT()
struct FZoneJob
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zone Job Config")
	int32 zoneID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Job Config")
	uint8 LOD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone Job Config")
	bool isInPlaceLODUpdate;
	FZoneJob(int32 aZoneID, uint8 aLOD, bool aisInPlaceLODUpdate) : zoneID{ aZoneID }, LOD{ aLOD }, isInPlaceLODUpdate{ aisInPlaceLODUpdate } {}
	FZoneJob() : zoneID{ -1 }, LOD{ 0 }, isInPlaceLODUpdate{ false } {}
};