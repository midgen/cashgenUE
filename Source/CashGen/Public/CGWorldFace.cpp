#include "cashgen.h"
#include "RuntimeMeshComponent.h"
#include "CGWorldFace.h"

ACGWorldFace::ACGWorldFace(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
}

ACGWorldFace::~ACGWorldFace()
{

}

void ACGWorldFace::BeginPlay()
{
	//String compName = "RMC";
	MeshComponent = NewObject<URuntimeMeshComponent>(this, URuntimeMeshComponent::StaticClass());//, *compName);
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MeshComponent->bShouldSerializeMeshData = false;
	MeshComponent->RegisterComponent();

}

void ACGWorldFace::RenderMesh()
{
	MeshComponent->CreateMeshSection(0, MyVertices, MyIndices, false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::CalculateNormalTangent);
}

void ACGWorldFace::SubDivide(const FRuntimeMeshVertexSimple &v1, const FRuntimeMeshVertexSimple &v2, const FRuntimeMeshVertexSimple &v3, const int32 aDepth, const float aScale)
{
	if (aDepth == 0) {
		MyVertices.Emplace(v1);
		MyVertices.Emplace(v2);
		MyVertices.Emplace(v3);

		MyIndices.Add(MyVertices.Num() - 3);
		MyIndices.Add(MyVertices.Num() - 2);
		MyIndices.Add(MyVertices.Num() - 1);
		return;
	}
	const FRuntimeMeshVertexSimple v12 = FRuntimeMeshVertexSimple(((v1.Position + v2.Position).GetSafeNormal()) * aScale);
	const FRuntimeMeshVertexSimple v23 = FRuntimeMeshVertexSimple(((v2.Position + v3.Position).GetSafeNormal()) * aScale);
	const FRuntimeMeshVertexSimple v31 = FRuntimeMeshVertexSimple(((v3.Position + v1.Position).GetSafeNormal()) * aScale);
	SubDivide(v1, v12, v31, aDepth - 1, aScale);
	SubDivide(v2, v23, v12, aDepth - 1, aScale);
	SubDivide(v3, v31, v23, aDepth - 1, aScale);
	SubDivide(v12, v23, v31, aDepth - 1, aScale);
}

void ACGWorldFace::Tick(float DeltaSeconds)
{

}

void ACGWorldFace::SetupFace(FRuntimeMeshVertexSimple v1, FRuntimeMeshVertexSimple v2, FRuntimeMeshVertexSimple v3, const FVector aOrigin, const int32 aDepth, const float aScale)
{
	SubDivide(v1, v2, v3, aDepth, aScale);

	RenderMesh();
}
