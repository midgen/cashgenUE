#pragma once

#include "cashgen.h"
#include "CGPoint.h"
#include "RuntimeMeshComponent.h"
#include "CGTile.generated.h"

class ACGTerrainManager;
struct FCGTerrainConfig;

enum ELODStatus { NOT_CREATED, CREATED, TRANSITION };

UCLASS()
class ACGTile : public AActor
{
	GENERATED_BODY()

	TMap<uint8, URuntimeMeshComponent*> MeshComponents;
	TMap<uint8, UMaterialInstanceDynamic*> MaterialInstances;
	UMaterialInstance* MaterialInstance;
	UMaterial* Material;
	TMap<uint8, ELODStatus> LODStatus;

	float LODTransitionOpacity = 0.0f;

	uint8 CurrentLOD;
	uint8 PreviousLOD;

	USphereComponent* SphereComponent;

public:
	ACGTile();
	~ACGTile();

public:

	CGPoint Offset;
	FVector WorldOffset;
	FCGTerrainConfig* TerrainConfigMaster;

	uint8 GetCurrentLOD();

	void RepositionAndHide(uint8 aNewLOD);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SetupTile(CGPoint aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset);
	void UpdateMesh(uint8 aLOD, bool aIsInPlaceUpdate, TArray<FVector>*	aVertices,
		TArray<int32>*		aTriangles,
		TArray<FVector>*	aNormals,
		TArray<FVector2D>*	aUV0,
		TArray<FColor>*		aVertexColors,
		TArray<FRuntimeMeshTangent>* aTangents);

	FVector GetCentrePos();

};