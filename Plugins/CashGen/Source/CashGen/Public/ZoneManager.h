// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "cashgen.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "Runtime/Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h"
#include "CGPoint.h"
#include "MeshData.h"
#include "ZoneConfig.h"
#include "ZoneManager.generated.h"

class AWorldManager;

enum eLODStatus { BUILDING_REQUIRES_CREATE, READY_TO_DRAW_REQUIRES_CREATE, BUILDING, READY_TO_DRAW, IDLE, PENDING_DRAW_REQUIRES_CREATE, PENDING_DRAW, DRAWING_REQUIRES_CREATE, DRAWING, SEGUE };

UCLASS()
class AZoneManager : public AActor
{
	GENERATED_BODY()
	// Map of the Runtime mesh components (we have one for each LOD)
	TMap<uint8, URuntimeMeshComponent*> MyRuntimeMeshComponents;

	// List of instanced mesh components for spawning foliage and other meshes
	TArray<UHierarchicalInstancedStaticMeshComponent*> MyInstancedMeshComponents;
	// For tracking blocks that still need to be processed for foliage spawning
	int32 MyBlocksToSpawnFoliageOn;

	// Map of mesh data for each LOD
	TMap<uint8, FMeshData> MyLODMeshData;

	// Zone configuration parameters
	FZoneConfig MyConfig;

	// Worker thread for building mesh data
	FRunnableThread* Thread;

	AWorldManager* MyWorldManager;

	FVector* worldOffset;

	const float SEGUE_DROP = 100000.0f;

	void PopulateMeshData(const uint8 aLOD);
	void CalculateTriangles(const uint8 aLOD);

	bool SpawnTreesAtIndex(int32* aIndex);
	bool GetGodCastHitPos(const FVector aVectorToStart, FVector* aHitPos, FVector* aNormalVector);

public:
	// Sets default values for this actor's properties
	AZoneManager();
	~AZoneManager();
	CGPoint MyOffset;
	TMap<uint8, eLODStatus> MyLODMeshStatus;
	uint8 currentlyDisplayedLOD;
	int32 MyZoneID;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	void SetupZone(const int32 aZoneID, AWorldManager* aWorldManager, const CGPoint aOffset, const FZoneConfig aZoneConfig, FVector* aWorldOffset);
	void RegenerateZone(const uint8 aLOD, const bool isInPlaceLODUpdate);
	void UpdateMesh(const uint8 aLOD);

	FVector GetCentrePos();
};
