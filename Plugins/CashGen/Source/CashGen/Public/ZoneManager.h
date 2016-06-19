// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "cashgen.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "Point.h"
#include "MeshData.h"
#include "ZoneConfig.h"
#include "ZoneManager.generated.h"

class AWorldManager;

enum eLODStatus { BUILDING_REQUIRES_CREATE, READY_TO_DRAW_REQUIRES_CREATE, BUILDING, READY_TO_DRAW, IDLE, PENDING_DRAW_REQUIRES_CREATE, PENDING_DRAW, DRAWING_REQUIRES_CREATE, DRAWING };

UCLASS()
class AZoneManager : public AActor
{
	GENERATED_BODY()
	// Map of the Runtime mesh components (we have one for each LOD)
	TMap<uint8, URuntimeMeshComponent*> MyRuntimeMeshComponents;

	// List of instanced mesh components for spawning foliage and other meshes
	TArray<UInstancedStaticMeshComponent*> MyInstancedMeshComponents;
	// Map of mesh data for each LOD
	TMap<uint8, FMeshData> MyLODMeshData;

	// Zone configuration parameters
	FZoneConfig MyConfig;

	// Worker thread for building mesh data
	FRunnableThread* Thread;

	AWorldManager* MyWorldManager;

	FVector* worldOffset;

	void PopulateMeshData(const uint8 aLOD);
	void CalculateTriangles(const uint8 aLOD);

public:
	// Sets default values for this actor's properties
	AZoneManager();
	~AZoneManager();
	Point MyOffset;
	TMap<uint8, eLODStatus> MyLODMeshStatus;
	uint8 currentlyDisplayedLOD;
	int32 MyZoneID;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	void SetupZone(const int32 aZoneID, AWorldManager* aWorldManager, const Point aOffset, const FZoneConfig aZoneConfig, FVector* aWorldOffset);
	void RegenerateZone(const uint8 aLOD, const bool isInPlaceLODUpdate);
	void UpdateMesh(const uint8 aLOD);

	FVector GetCentrePos();
};
