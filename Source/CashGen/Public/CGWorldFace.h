#pragma once
#include "cashgen.h"
#include "CGWorldConfig.h"
#include "CGWorldFace.generated.h"


class URuntimeMeshComponent;
class ACGWorldFace;
class ACGWorld;

UCLASS()
class ACGWorldFace : public AActor
{
	GENERATED_BODY()

	URuntimeMeshComponent* MeshComponent;
	USphereComponent* SphereComponent;

	TArray<FRuntimeMeshVertexSimple> MyVertices;
	TArray<int32> MyIndices;

	ACGWorldFace* MyParentFace;
	ACGWorld* MyWorld;

	void SubDivideGeometry(const FRuntimeMeshVertexSimple &v1, const FRuntimeMeshVertexSimple &v2, const FRuntimeMeshVertexSimple &v3, const int32 aDepth, const float aScale);

public:
	ACGWorldFace(const FObjectInitializer& ObjectInitializer);
	~ACGWorldFace();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	void SetupFace(FRuntimeMeshVertexSimple v1, FRuntimeMeshVertexSimple v2, FRuntimeMeshVertexSimple v3, const FVector aOrigin, int32 aDepth, const int32 aSubDivLimit, const float aScale, ACGWorld* aWorld, ACGWorldFace* aParentFace);
	void UpdateMesh(TArray<FRuntimeMeshVertexSimple>& aVertices, TArray<int32>& aIndices);
	void RenderMesh();

	
};