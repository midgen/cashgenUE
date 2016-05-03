// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ZoneManager.h"
#include "Point.h"
#include "ZoneConfig.h"
#include "ZoneJob.h"
#include "WorldManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AWorldManager : public AActor
{
	GENERATED_BODY()
	// All our child zones
	FZoneConfig MyZoneConfigMaster;
	TArray<AZoneManager*> ZonesMaster;
	int32 MyNumXZones;
	int32 MyNumYZones;

	int32 MyMaxThreads;

	ACharacter* currentPlayerPawn;
	UWorld* world;

	uint8 RenderTokens;
	float TimeSinceLastSweep;
	const float SweepInterval = 1.0f;

	// Queue for managing zone generation jobs
	TArray<FZoneJob> MyRegenQueue;

	// Couple of helper functions for accessing the zone array using coords
	int32 GetIdxfromXY(const Point point) { return point.x * MyNumYZones + point.y; };
	Point GetXYfromIdx(const int32 idx) { return Point(idx % MyNumYZones, idx / MyNumYZones); }

	// Called when the player pawn crosses a zone boundary
	void HandleZoneChange(const FVector2D delta);

	bool isSetup;

public:
	AWorldManager();
	// Queues for managing the number of threads
	TQueue<uint8, EQueueMode::Mpsc> MyAvailableThreads;
	// Queue for managing mesh updates
	TQueue<FZoneJob, EQueueMode::Mpsc> MyRenderQueue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen Manager")
		FVector2D currentPlayerZone;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	uint8 GetLODForOffset(const Point aOffset);
	uint8 GetLODForZoneManagerIndex(const int32 aZoneIndex);

	UFUNCTION(BlueprintCallable, Category = "World Manager")
	void SpawnZones(ACharacter* aPlayerPawn, const FZoneConfig aZoneConfig, const int32 aNumXZones, const int32 aNumYZones, const int32 aMaxThreads, const uint8 aRenderTokens);

	void CreateZoneRefreshJob(const int32 aZoneIndex, const uint8 aLOD, const bool aIsInPlaceLODUpdate);


};
