#pragma once
#include "CoreMinimal.h"
#include "CGWorldConfig.h"
#include "CGWorldFace.generated.h"


class URuntimeMeshComponent;
class ACGWorldFace;
class ACGWorld;

UCLASS()
class CASHGEN_API ACGWorldFace : public AActor
{
	GENERATED_BODY()

	URuntimeMeshComponent* myMeshComponent;
	USphereComponent* mySphereComponent;

	UMaterial* myMaterial;
	UMaterialInstanceDynamic* myMaterialInstanceDynamic;

	TArray<FRuntimeMeshVertexSimple> myVertices;
	TArray<int32> myIndices;

	ACGWorldFace* myParentFace;
	ACGWorld* myWorld;

	void SubDivideGeometry(const FRuntimeMeshVertexSimple &v1, const FRuntimeMeshVertexSimple &v2, const FRuntimeMeshVertexSimple &v3, const int32 aDepth, const float aScale);

public:
	ACGWorldFace(const FObjectInitializer& ObjectInitializer);
	~ACGWorldFace();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	void SetupFace(FRuntimeMeshVertexSimple v1, FRuntimeMeshVertexSimple v2, FRuntimeMeshVertexSimple v3, const FVector aOrigin, int32 aDepth, const int32 aSubDivLimit, const float aScale, ACGWorld* aWorld, ACGWorldFace* aParentFace);
	void UpdateMesh(TArray<FRuntimeMeshVertexSimple>& aVertices, TArray<int32>& aIndices, FCGWorldConfig& aWorldConfig);
	UMaterialInstanceDynamic* GetMaterialInstanceDynamic();

	
};
