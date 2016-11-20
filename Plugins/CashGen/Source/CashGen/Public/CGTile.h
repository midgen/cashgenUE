#pragma once

#include "cashgen.h"
#include "CGPoint.h"
#include "Struct/CGMeshData.h"
#include "Struct/CGTerrainConfig.h"
#include "RuntimeMeshComponent.h"
#include "CGTile.generated.h"

class ACGTerrainManager;

enum ELODStatus { NOT_CREATED, CREATED };

UCLASS()
class ACGTile : public AActor
{
	GENERATED_BODY()

	TMap<uint8, URuntimeMeshComponent*> MeshComponents;
	TMap<uint8, UMaterialInstanceDynamic*> MaterialInstances;
	TMap<uint8, ELODStatus> LODStatus;

	float LODTransitionOpacity = 0.0f;

public:
	ACGTile();
	~ACGTile();

	CGPoint Offset;
	FVector WorldOffset;
	FCGTerrainConfig* TerrainConfigMaster;

	uint8 CurrentLOD;
	uint8 PreviousLOD;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SetupTile(CGPoint aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset);
	void UpdateMesh(uint8 aLOD, FCGMeshData* aMeshData);

	FVector GetCentrePos();

};