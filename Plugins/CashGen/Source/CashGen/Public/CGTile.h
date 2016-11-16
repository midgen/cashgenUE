#pragma once

#include "cashgen.h"
#include "CGPoint.h"
#include "Struct/CGMeshData.h"
#include "Struct/CGTerrainConfig.h"
#include "RuntimeMeshComponent.h"
#include "CGTile.generated.h"

class ACGTerrainManager;

//enum ETileStatus { IDLE, SEGUE };

UCLASS()
class ACGTile : public AActor
{
	GENERATED_BODY()

	TMap<uint8, URuntimeMeshComponent*> MeshComponents;
	TMap<uint8, UMaterialInstanceDynamic*> MaterialInstances;

	float LODTransitionOpacity = 0.0f;

public:
	ACGTile();
	~ACGTile();

	CGPoint Offset;
	FCGTerrainConfig* TerrainConfigMaster;

	uint8 CurrentLOD;
	uint8 PreviousLOD;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SetupZone(CGPoint aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset);
	void UpdateMesh(uint8 aLOD, FCGMeshData* aMeshData);

	FVector GetCentrePos();

};