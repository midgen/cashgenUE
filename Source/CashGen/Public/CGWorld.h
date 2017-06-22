#pragma once
#include "cashgen.h"
#include "CGWorldConfig.h"
#include "CGWorld.generated.h"


class URuntimeMeshComponent;
class ACGWorldFace;

UCLASS()
class ACGWorld : public AActor
{
	GENERATED_BODY()

	URuntimeMeshComponent* MeshComponent;
	//UMaterialInstanceDynamic* MaterialInstance;
	USphereComponent* SphereComponent;

	TArray<FRuntimeMeshVertexSimple> MyVertices;
	TArray<int32> MyIndices;

	TArray<ACGWorldFace*> MyFaces;

public:
	ACGWorld(const FObjectInitializer& ObjectInitializer);
	~ACGWorld();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CGWorld")
	FCGWorldConfig WorldConfig;

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

	void RenderMesh();

	void InitializeSphere(FRuntimeMeshComponentVerticesBuilder* aVertices, FRuntimeMeshIndicesBuilder* aIndices, const int32 aDepth, const float aScale);
};