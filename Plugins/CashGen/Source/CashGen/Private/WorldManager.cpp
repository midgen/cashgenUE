// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgen.h"
#include "WorldManager.h"
#include "ZoneConfig.h"

AWorldManager::AWorldManager()
{
	PrimaryActorTick.bCanEverTick = true;

	miniMapGenerator = CreateDefaultSubobject<UMiniMapGenerator>(FName("MiniMapGenerator"));
}

void AWorldManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWorldManager::Tick(float DeltaTime )
{
	Super::Tick( DeltaTime );

	TimeSinceLastSweep += DeltaTime;

	if (isSetup)
	{
		FVector2D oldPos = currentPlayerZone;

		if (currentPlayerPawn)
		{
			currentPlayerZone.X = floor(currentPlayerPawn->GetActorLocation().X / (MyZoneConfigMaster.UnitSize * MyZoneConfigMaster.XUnits));
			currentPlayerZone.Y = floor(currentPlayerPawn->GetActorLocation().Y / (MyZoneConfigMaster.UnitSize * MyZoneConfigMaster.YUnits));
		}

		FVector2D newPos = currentPlayerZone;

		if (oldPos.X != newPos.X || oldPos.Y != newPos.Y)
		{
			HandleZoneChange(newPos - oldPos);
		}

		FZoneJob renderJob;
		uint8 tokensRemaining = RenderTokens;

		while (tokensRemaining > 0)
		{
			if (MyRenderQueue.Dequeue(renderJob))
			{
				if (ZonesMaster[renderJob.zoneID]->MyLODMeshStatus[renderJob.LOD] == PENDING_DRAW)
				{
					ZonesMaster[renderJob.zoneID]->MyLODMeshStatus[renderJob.LOD] = DRAWING;
					ZonesMaster[renderJob.zoneID]->UpdateMesh(renderJob.LOD);
				}
				else if (ZonesMaster[renderJob.zoneID]->MyLODMeshStatus[renderJob.LOD] == PENDING_DRAW_REQUIRES_CREATE)
				{
					ZonesMaster[renderJob.zoneID]->MyLODMeshStatus[renderJob.LOD] = DRAWING_REQUIRES_CREATE;
					ZonesMaster[renderJob.zoneID]->UpdateMesh(renderJob.LOD);
				}
				tokensRemaining -= 4 - renderJob.LOD;
			}
			else {
				tokensRemaining = 0;
			}
		}

		uint8 dummy;
		while (MyRegenQueue.Num() > 0 && MyAvailableThreads.Dequeue(dummy))
		{
			ZonesMaster[MyRegenQueue.Last().zoneID]->RegenerateZone(MyRegenQueue.Last().LOD, MyRegenQueue.Last().isInPlaceLODUpdate);
			// If it's a 10 (don't render) LOD, give the thread back to the queue
			if (MyRegenQueue.Last().LOD == 10) {
				MyAvailableThreads.Enqueue(1);
			}
			MyRegenQueue.RemoveAt(MyRegenQueue.Num() - 1);
		}

		// Sweep the zones for LOD changes
		if (TimeSinceLastSweep > SweepInterval)
		{
			for (int i = 0; i < ZonesMaster.Num(); ++i)
			{
				uint8 newLOD = GetLODForZoneManagerIndex(i);
				if (ZonesMaster[i]->currentlyDisplayedLOD > newLOD)
				{
					bool IsAlreadyQueued = false;
					for (auto& job : MyRegenQueue) {
						if (job.zoneID == i) {
							IsAlreadyQueued = true;
							break;
						}
					}
					if (!IsAlreadyQueued) {
						CreateZoneRefreshJob(i, newLOD, true);
					}
				}
			}
			TimeSinceLastSweep = 0.0f;
		}
	}
}

void AWorldManager::HandleZoneChange(const FVector2D delta)
{
	int32 minX = 0;
	int32 maxX = 0;
	int32 minY = 0;
	int32 maxY = 0;

	// TODO: Replace all this with a distance based lifecycle state machine for LOD implementation

	for (int i = 0; i < ZonesMaster.Num(); ++i)
	{
		if (i == 0) {
			minX = ZonesMaster[i]->MyOffset.x;
			maxX = ZonesMaster[i]->MyOffset.x;
			minY = ZonesMaster[i]->MyOffset.y;
			maxY = ZonesMaster[i]->MyOffset.y;
		}
		if (ZonesMaster[i]->MyOffset.x < minX) {
			minX = ZonesMaster[i]->MyOffset.x;
		}
		if (ZonesMaster[i]->MyOffset.x > maxX) {
			maxX = ZonesMaster[i]->MyOffset.x;
		}
		if (ZonesMaster[i]->MyOffset.y < minY) {
			minY = ZonesMaster[i]->MyOffset.y;
		}
		if (ZonesMaster[i]->MyOffset.y > maxY) {
			maxY = ZonesMaster[i]->MyOffset.y;
		}
	}

	for (int i = 0; i < ZonesMaster.Num(); ++i)
	{
		// Moving left on X axis, flip left column to the right
		if (delta.X < -0.1 && ZonesMaster[i]->MyOffset.x == maxX)
		{
			ZonesMaster[i]->MyOffset.x = minX - 1;
			CreateZoneRefreshJob(i, GetLODForZoneManagerIndex(i), false);
		}
		// Moving right on X, flip right column to left
		if (delta.X > 0.1 && ZonesMaster[i]->MyOffset.x == minX)
		{
			ZonesMaster[i]->MyOffset.x = maxX + 1;
			CreateZoneRefreshJob(i, GetLODForZoneManagerIndex(i), false);
		}
		// Movin down on Y, flip top row to bottom
		if (delta.Y < -0.1 && ZonesMaster[i]->MyOffset.y == maxY)
		{
			ZonesMaster[i]->MyOffset.y = minY - 1;
			CreateZoneRefreshJob(i, GetLODForZoneManagerIndex(i), false);
		}
		// Moving up on Y, flip bottom wor to top
		if (delta.Y > 0.1 && ZonesMaster[i]->MyOffset.y == minY)
		{
			ZonesMaster[i]->MyOffset.y = maxY + 1;
			CreateZoneRefreshJob(i, GetLODForZoneManagerIndex(i), false);
		}
	}

	// TODO: Implement the minimap generator
	//RefreshMiniMapTexture();
}

void AWorldManager::CreateZoneRefreshJob(const int32 aZoneIndex, const uint8 aLOD, const bool aIsInPlaceLODUpdate)
{
	MyRegenQueue.Add(FZoneJob(aZoneIndex, aLOD, aIsInPlaceLODUpdate));
}



void AWorldManager::RefreshMiniMapTexture()
{
	if (miniMapGenerator)
	{
		miniMapGenerator->UpdateHeightmapTexture();
		NotifiyMiniMapUpdated();
	}
}


uint8 AWorldManager::GetLODForZoneManagerIndex(const int32 aZoneIndex)
{
	FVector diff;

	FVector centreOfZone = ZonesMaster[aZoneIndex]->GetCentrePos();


	diff = currentPlayerPawn->GetActorLocation() - centreOfZone;

	float distance = diff.Size();

	if (distance <= MyZoneConfigMaster.LOD1Range)
	{
		return 0;
	}
	if (distance > MyZoneConfigMaster.LOD1Range && distance <= MyZoneConfigMaster.LOD2Range)
	{
		return 1;
	}
	if (distance > MyZoneConfigMaster.LOD2Range && distance <= MyZoneConfigMaster.LODCullRange)
	{
		return 2;
	}
	if (distance > MyZoneConfigMaster.LODCullRange)
	{
		return 10;
	}
	return 0;
}

uint8 AWorldManager::GetLODForOffset(const CGPoint aOffset)
{
	return 0;
}

void AWorldManager::SpawnZones(APawn* aPlayerPawn, const FZoneConfig aZoneConfig, const int32 aNumXZones, const int32 aNumYZones, const int32 aMaxThreads, const uint8 aRenderTokens)
{
	world = GetWorld();
	MyZoneConfigMaster = aZoneConfig;
	MyNumXZones = aNumXZones;
	MyNumYZones = aNumYZones;
	MyMaxThreads = aMaxThreads;
	RenderTokens = aRenderTokens;

	currentPlayerPawn = aPlayerPawn;

	miniMapGenerator->Init(this, aZoneConfig);
	miniMapGenerator->CreateHeightmapTexture();

	for (int i = 0; i < MyMaxThreads; ++i)
	{
		MyAvailableThreads.Enqueue(1);
	}

	worldOffset = FVector((MyNumXZones / 2.0f) * MyZoneConfigMaster.XUnits * MyZoneConfigMaster.UnitSize, (MyNumYZones / 2.0f) * MyZoneConfigMaster.YUnits * MyZoneConfigMaster.UnitSize, 0.0f);

	if (currentPlayerPawn)
	{
		currentPlayerZone.X = floor(currentPlayerPawn->GetActorLocation().X / ((MyZoneConfigMaster.UnitSize * MyZoneConfigMaster.XUnits) - worldOffset.X));
		currentPlayerZone.Y = floor(currentPlayerPawn->GetActorLocation().Y / ((MyZoneConfigMaster.UnitSize * MyZoneConfigMaster.YUnits) - worldOffset.Y));
	}

	for (int32 i = 0; i < MyNumXZones * MyNumYZones; ++i )
	{
		ZonesMaster.Add(world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), FVector((MyZoneConfigMaster.XUnits * MyZoneConfigMaster.UnitSize * GetXYfromIdx(i).x) - worldOffset.X, (MyZoneConfigMaster.YUnits * MyZoneConfigMaster.UnitSize * GetXYfromIdx(i).y) - worldOffset.Y, 0.0f) - worldOffset, FRotator(0.0f)));
	}

	for (int i = 0; i < ZonesMaster.Num(); ++i)
	{
		ZonesMaster[i]->SetupZone(i, this, GetXYfromIdx(i), MyZoneConfigMaster, &worldOffset);
		CreateZoneRefreshJob(i, GetLODForZoneManagerIndex(i), false);
	}

	isSetup = true;
}



