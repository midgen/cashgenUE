#pragma once

#include "cashgen.h"
#include "IntVector2.h"
#include "RuntimeMeshComponent.h"
#include "CGTile.generated.h"

class ACGTerrainManager_Legacy;
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

	bool IsInitalized = false;


public:
	ACGTile();
	~ACGTile();

public:

	FIntVector2 mySector;
	

	FVector WorldOffset;
	FCGTerrainConfig* TerrainConfigMaster;

	uint8 GetCurrentLOD();

	void RepositionAndHide(uint8 aNewLOD);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SetupTile(FIntVector2 aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset);
	void UpdateMesh(uint8 aLOD, bool aIsInPlaceUpdate, TArray<FVector>*	aVertices,
		TArray<int32>*		aTriangles,
		TArray<FVector>*	aNormals,
		TArray<FVector2D>*	aUV0,
		TArray<FColor>*		aVertexColors,
		TArray<FRuntimeMeshTangent>* aTangents);

	FVector GetCentrePos();

	UMaterialInstanceDynamic* GetMaterialInstanceDynamic(const uint8 aLOD);

};