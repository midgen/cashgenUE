// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ZoneManager.h"
#include "CGPoint.h"
#include "ZoneConfig.h"
#include "ZoneJob.h"
#include "MiniMapGenerator.h"
#include "WorldManager.generated.h"

#define WORLD_HEIGHTMAP_TEXTURE_SIZE 1024
#define WORLD_HEIGHTMAP_DATA_LENGTH WORLD_HEIGHTMAP_TEXTURE_SIZE * WORLD_HEIGHTMAP_TEXTURE_SIZE

UCLASS(BlueprintType, Blueprintable)
class AWorldManager : public AActor
{
	GENERATED_BODY()
	// Master list of child zones
	TArray<AZoneManager*> ZonesMaster;
	int32 MyNumXZones;
	int32 MyNumYZones;
	// Max Threads
	int32 MyMaxThreads;
	// Pawn to track
	APawn* currentPlayerPawn;
	UWorld* world;

	uint8 RenderTokens;

	// for controlling LOD sweeps
	float TimeSinceLastSweep;
	const float SweepInterval = 1.0f;

	// Queue for managing zone generation jobs
	TArray<FZoneJob> MyRegenQueue;

	// Couple of helper functions for accessing the zone array using coords
	int32 GetIdxfromXY(const CGPoint point) { return point.x * MyNumYZones + point.y; };
	CGPoint GetXYfromIdx(const int32 idx) { return CGPoint(idx % MyNumYZones, idx / MyNumYZones); }

	// Called when the player pawn crosses a zone boundary
	void HandleZoneChange(const FVector2D delta);

	bool isSetup;

public:
	AWorldManager();
	// Queues for managing the number of threads
	TQueue<uint8, EQueueMode::Mpsc> MyAvailableThreads;
	// Queue for managing mesh updates
	TQueue<FZoneJob, EQueueMode::Mpsc> MyRenderQueue;
	// Master zone Configuration
	FZoneConfig MyZoneConfigMaster;

	FVector worldOffset;

	UPROPERTY(Instanced)
	UMiniMapGenerator* miniMapGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen Manager")
	FVector2D currentPlayerZone;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	uint8 GetLODForOffset(const CGPoint aOffset);
	uint8 GetLODForZoneManagerIndex(const int32 aZoneIndex);

	UFUNCTION(BlueprintCallable, Category = "World Manager")
	void SpawnZones(APawn* aPlayerPawn, const FZoneConfig aZoneConfig, const int32 aNumXZones, const int32 aNumYZones, const int32 aMaxThreads, const uint8 aRenderTokens);

	void CreateZoneRefreshJob(const int32 aZoneIndex, const uint8 aLOD, const bool aIsInPlaceLODUpdate);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Manager")
	UTexture2D* MiniMapTexture;

	UFUNCTION(BlueprintCallable, Category = "World Manager")
	void RefreshMiniMapTexture();

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldManager")
	void NotifiyMiniMapUpdated();

};
