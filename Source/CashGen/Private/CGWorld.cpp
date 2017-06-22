#include "cashgen.h"
#include "RuntimeMeshComponent.h"
#include "CGWorldFace.h"
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

	//MeshComponent->CreateMeshSection(0, MyVertices, MyIndices, false, EUpdateFrequency::Infrequent, ESectionUpdateFlags::CalculateNormalTangent);
}

void ACGWorld::AddFace(ACGWorldFace* aFace)
{
	MyFaces.Add(aFace);
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

	int32 indices[] = { 0,4,1, 0,9,4, 9,5,4, 4,5,8, 4,8,1, 8,10,1, 8,3,10, 5,3,8, 5,2,3, 2,7,3, 7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6, 6,1,10, 9,0,11, 9,11,2, 9,2,5, 7,2,11 };

	MyIndices.Append(indices, ARRAY_COUNT(indices));

	UWorld* _world = GetWorld();


	for (int i = 0; i < 20; i++)
	{
		MyFaces.Add(_world->SpawnActor<ACGWorldFace>(ACGWorldFace::StaticClass(), FVector(0.0f), FRotator(0.0f)));
		MyFaces[MyFaces.Num() - 1]->SetupFace(MyVertices[MyIndices[i * 3]],
												MyVertices[MyIndices[(i * 3) + 1]],
												MyVertices[MyIndices[(i * 3) + 2]], 
												FVector(0.0f),
												WorldConfig.DebugSubdivisions ? i : WorldConfig.Subdivisions,
												WorldConfig.Radius,
												this,
												nullptr);
	}
	
	//for (int i = 0; i < 20; i++)
		//SubDivide(MyVertices[MyIndices[(i*3)+0]], MyVertices[MyIndices[(i*3) + 1]], MyVertices[MyIndices[(i*3) + 2]], aDepth, aScale);
}

void ACGWorld::Tick(float DeltaSeconds)
{

}