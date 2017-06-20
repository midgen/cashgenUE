#include "cashgen.h"
#include "RuntimeMeshComponent.h"
#include "CGWorld.h"

ACGWorld::ACGWorld(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
}

ACGWorld::~ACGWorld()
{

}

void ACGWorld::BeginPlay()
{
	FString compName = "RMC";
	MeshComponent = NewObject<URuntimeMeshComponent>(this, URuntimeMeshComponent::StaticClass(), *compName);
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MeshComponent->bShouldSerializeMeshData = false;
	MeshComponent->RegisterComponent();

	RenderMesh();
}

void ACGWorld::RenderMesh()
{
	FRuntimeMeshComponentVerticesBuilder vertices(true, false, false, false, false);
	FRuntimeMeshIndicesBuilder indices;


	InitializeSphere(&vertices, &indices, WorldConfig.Subdivisions, WorldConfig.Radius);

	MeshComponent->CreateMeshSection(0, MyVertices, MyIndices, false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::CalculateNormalTangent);
}

void ACGWorld::InitializeSphere(FRuntimeMeshComponentVerticesBuilder* aVertices, FRuntimeMeshIndicesBuilder* aIndices, const int32 aDepth, const float aScale)
{
	const float X = 0.525731112119133606f * aScale;
	const float Z = 0.850650808352039932f * aScale;
	
	MyVertices.Emplace(FVector(-X, 0.0f, Z));
	MyVertices.Emplace(FVector(X, 0.0f, Z));
	MyVertices.Emplace(FVector(-X, 0.0f, -Z));
	MyVertices.Emplace(FVector(X, 0.0f, -Z));
	MyVertices.Emplace(FVector(0.0f, Z, X));
	MyVertices.Emplace(FVector(0.0f, Z, -X));
	MyVertices.Emplace(FVector(0.0f, -Z, X));
	MyVertices.Emplace(FVector(0.0f, -Z, -X));
	MyVertices.Emplace(FVector(Z, X, 0.0f));
	MyVertices.Emplace(FVector(-Z, X, 0.0f));
	MyVertices.Emplace(FVector(Z, -X, 0.0f));
	MyVertices.Emplace(FVector(-Z, -X, 0.0f));


	//aVertices->SetPosition(0, FVector(-X,    0.0f,  Z));
	//aVertices->SetPosition(1, FVector(X,     0.0f,  Z));
	//aVertices->SetPosition(2, FVector(-X,    0.0f, -Z));
	//aVertices->SetPosition(3, FVector(X,     0.0f, -Z));
	//aVertices->SetPosition(4, FVector(0.0f,  Z,     X));
	//aVertices->SetPosition(5, FVector(0.0f,  Z,    -X));
	//aVertices->SetPosition(6, FVector(0.0f, -Z,     X));
	//aVertices->SetPosition(7, FVector(0.0f, -Z,    -X));
	//aVertices->SetPosition(8, FVector(Z,     X,  0.0f));
	//aVertices->SetPosition(9, FVector(-Z,    X,  0.0f));
	//aVertices->SetPosition(10,FVector(Z,    -X,  0.0f));
	//aVertices->SetPosition(11,FVector(-Z,   -X,  0.0f));
	

	//aVertices->SetNormal(0, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(1, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(2, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(3, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(4, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(5, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(6, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(7, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(8, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(9, FVector(0.0f, 0.0f, 1.0f));
	//aVertices->SetNormal(10, FVector(0.0f, 0.0f, 1.0));
	//aVertices->SetNormal(11, FVector(0.0f, 0.0f, 1.0));

	int32 indices[] = { 0,4,1, 0,9,4, 9,5,4, 4,5,8, 4,8,1, 8,10,1, 8,3,10, 5,3,8, 5,2,3, 2,7,3, 7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6, 6,1,10, 9,0,11, 9,11,2, 9,2,5, 7,2,11 };

	MyIndices.Append(indices, ARRAY_COUNT(indices));

	//aIndices->AddTriangle(0, 4, 1);
	//aIndices->AddTriangle(0, 9, 4);
	//aIndices->AddTriangle(9, 5, 4);
	//aIndices->AddTriangle(4, 5, 8);
	//aIndices->AddTriangle(4, 8, 1);
	//aIndices->AddTriangle(8, 10, 1);
	//aIndices->AddTriangle(8, 3, 10);
	//aIndices->AddTriangle(5, 3, 8);
	//aIndices->AddTriangle(5, 2, 3);
	//aIndices->AddTriangle(2, 7, 3);
	//aIndices->AddTriangle(7, 10, 3);
	//aIndices->AddTriangle(7, 6, 10);
	//aIndices->AddTriangle(7, 11, 6);
	//aIndices->AddTriangle(11, 0, 6);
	//aIndices->AddTriangle(0, 1, 6);
	//aIndices->AddTriangle(6, 1, 10);
	//aIndices->AddTriangle(9, 0, 11);
	//aIndices->AddTriangle(9, 11, 2);
	//aIndices->AddTriangle(9, 2, 5);
	//aIndices->AddTriangle(7, 2, 11);


	
	
		
	for (int i = 0; i < aDepth; i++)
		SubDivide(aVertices->GetPosition(aIndices->GetIndex(i+0)), aVertices->GetPosition(aIndices->GetIndex(i + 0)), aVertices->GetPosition(aIndices->GetIndex(i + 0)), aDepth);
}

void ACGWorld::SubDivide(const FVector &v1, const FVector &v2, const FVector &v3, const int32 aDepth)
{
	if (aDepth == 0) {
		MyVertices.Emplace(v1);
		MyVertices.Emplace(v2);
		MyVertices.Emplace(v3);
		return;
	}
	const FVector v12 = (v1 + v2).GetSafeNormal();
	const FVector v23 = (v2 + v3).GetSafeNormal();
	const FVector v31 = (v3 + v1).GetSafeNormal();
	SubDivide(v1, v12, v31, aDepth - 1);
	SubDivide(v2, v23, v12, aDepth - 1);
	SubDivide(v3, v31, v23, aDepth - 1);
	SubDivide(v12, v23, v31, aDepth - 1);
}

void ACGWorld::Tick(float DeltaSeconds)
{

}