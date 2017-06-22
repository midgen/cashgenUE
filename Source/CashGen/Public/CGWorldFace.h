#pragma once
#include "cashgen.h"
#include "CGWorldConfig.h"
#include "CGWorldFace.generated.h"


class URuntimeMeshComponent;
class ACGWorldFace;

UCLASS()
class ACGWorldFace : public AActor
{
	GENERATED_BODY()

	URuntimeMeshComponent* MeshComponent;
	USphereComponent* SphereComponent;

	TArray<FRuntimeMeshVertexSimple> MyVertices;
	TArray<int32> MyIndices;

	void SubDivide(const FRuntimeMeshVertexSimple &v1, const FRuntimeMeshVertexSimple &v2, const FRuntimeMeshVertexSimple &v3, const int32 aDepth, const float aScale);

public:
	ACGWorldFace(const FObjectInitializer& ObjectInitializer);
	~ACGWorldFace();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	void SetupFace(FRuntimeMeshVertexSimple v1, FRuntimeMeshVertexSimple v2, FRuntimeMeshVertexSimple v3, const FVector aOrigin, const int32 aDepth, const float aScale);

	void RenderMesh();

	
};