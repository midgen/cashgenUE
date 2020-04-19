#pragma once

#include "Cashgen/Public/Struct/IntVector2.h"

#include <Runtime/Engine/Classes/Components/SphereComponent.h>
#include <ProceduralMeshComponent/Public/ProceduralMeshComponent.h>

#include "CGTile.generated.h"

class UStaticMeshComponent;
struct FCGTerrainConfig;

UENUM(BlueprintType)
enum class ELODStatus : uint8
{ 
	NOT_CREATED, 
	CREATED, 
	TRANSITION
};

UCLASS()
class ACGTile : public AActor
{
	GENERATED_BODY()

	TMap<uint8, UProceduralMeshComponent*> MeshComponents;
	TMap<uint8, UMaterialInstanceDynamic*> MaterialInstances;
	UStaticMeshComponent* MyWaterMeshComponent;
	UMaterialInstance* MaterialInstance;
	UMaterialInstanceDynamic* myWaterMaterialInstance;
	UMaterial* Material;
	TMap<uint8, ELODStatus> LODStatus;

	float LODTransitionOpacity = 0.0f;

	uint8 CurrentLOD;
	uint8 PreviousLOD;

	USphereComponent* SphereComponent;

	bool IsInitalized = false;

	FIntVector2 mySector;

	FVector WorldOffset;
	FCGTerrainConfig* TerrainConfigMaster;

	UPROPERTY()
	UTexture2D* myTexture;

	FUpdateTextureRegion2D* myRegion;


public:
	ACGTile();
	~ACGTile();

public:

	bool TickTransition(float DeltaSeconds);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void UpdateSettings(FIntVector2 aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset);
	void UpdateMesh(uint8 aLOD, bool aIsInPlaceUpdate, TArray<FVector>& aPosition, TArray<FVector>& aNormals, TArray<FProcMeshTangent>& aTangents, TArray<FVector2D>& aUV0s, TArray<FColor>& aColours, TArray<int32>& aTriangles, TArray<FColor>& aTextureData);
	void RepositionAndHide(uint8 aNewLOD);

	bool CreateWaterMesh();

	

	UMaterialInstanceDynamic* GetMaterialInstanceDynamic(const uint8 aLOD);

};
