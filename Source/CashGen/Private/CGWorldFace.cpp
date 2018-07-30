#include "CGWorldFace.h"
#include "RuntimeMeshComponent.h"
#include "CGWorldFaceJob.h"


ACGWorldFace::ACGWorldFace(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	mySphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = mySphereComponent;
}

ACGWorldFace::~ACGWorldFace()
{

}

void ACGWorldFace::BeginPlay()
{
	//String compName = "RMC";
	myMeshComponent = NewObject<URuntimeMeshComponent>(this, URuntimeMeshComponent::StaticClass());//, *compName);
	myMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	myMeshComponent->BodyInstance.SetResponseToAllChannels(ECR_Block);
	myMeshComponent->RegisterComponent();

}

void ACGWorldFace::SubDivideGeometry(const FRuntimeMeshVertexSimple &v1, const FRuntimeMeshVertexSimple &v2, const FRuntimeMeshVertexSimple &v3, const int32 aDepth, const float aScale)
{
	if (aDepth == 0) {
		myVertices.Emplace(v1);
		myVertices.Emplace(v2);
		myVertices.Emplace(v3);

		myIndices.Add(myVertices.Num() - 3);
		myIndices.Add(myVertices.Num() - 2);
		myIndices.Add(myVertices.Num() - 1);
		return;
	}
	const FRuntimeMeshVertexSimple v12 = FRuntimeMeshVertexSimple(((v1.Position + v2.Position).GetSafeNormal()) * aScale);
	const FRuntimeMeshVertexSimple v23 = FRuntimeMeshVertexSimple(((v2.Position + v3.Position).GetSafeNormal()) * aScale);
	const FRuntimeMeshVertexSimple v31 = FRuntimeMeshVertexSimple(((v3.Position + v1.Position).GetSafeNormal()) * aScale);
	SubDivideGeometry(v1, v12, v31, aDepth - 1, aScale);
	SubDivideGeometry(v2, v23, v12, aDepth - 1, aScale);
	SubDivideGeometry(v3, v31, v23, aDepth - 1, aScale);
	SubDivideGeometry(v12, v23, v31, aDepth - 1, aScale);
}

void ACGWorldFace::Tick(float DeltaSeconds)
{


}

void ACGWorldFace::SetupFace(FRuntimeMeshVertexSimple v1, FRuntimeMeshVertexSimple v2, FRuntimeMeshVertexSimple v3, const FVector aOrigin, int32 aSubDivisions , const int32 aSubDivLimit, const float aRadius, ACGWorld* aWorld, ACGWorldFace* aParentFace)
{
	myWorld = aWorld;
	myParentFace = aParentFace;
	// We need to spawn more faces rather than try to do this all at once
	if (aSubDivisions > aSubDivLimit)
	{
		// Subdivide once to get all our points
		SubDivideGeometry(v1, v2, v3, 1, aRadius);
		aSubDivisions = aSubDivisions - aSubDivLimit;

		UWorld* _world = GetWorld();

		for (int i = 0; i < 4; i++)
		{
			ACGWorldFace* _newFace = _world->SpawnActor<ACGWorldFace>(ACGWorldFace::StaticClass(), FVector(0.0f), FRotator(0.0f));
			myWorld->AddFace(_newFace);

			_newFace->SetupFace(myVertices[myIndices[i * 3]],
				myVertices[myIndices[(i * 3) + 1]],
				myVertices[myIndices[(i * 3) + 2]],
				FVector(0.0f),
				aSubDivisions,
				aSubDivLimit,
				aRadius, 
				aWorld,
				this);
		}
	}
	else
	{
		// Otherwise, we will be rendering this face, so enqueue a generation job
		FCGWorldFaceJob newJob;
		newJob.pFace = this;
		newJob.Radius = aRadius;
		newJob.v1 = v1;
		newJob.v2 = v2;
		newJob.v3 = v3;
		newJob.SubDivisions = aSubDivisions;
		aWorld->PendingJobs.Enqueue(newJob);
	}
}

void ACGWorldFace::UpdateMesh(TArray<FRuntimeMeshVertexSimple>& aVertices, TArray<int32>& aIndices, FCGWorldConfig& aWorldConfig)
{
	myMeshComponent->CreateMeshSection(0, aVertices, aIndices, aWorldConfig.CollisionEnabled,  EUpdateFrequency::Infrequent);
	if (aWorldConfig.MakeDynamicInstance)
	{
		myMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(aWorldConfig.TerrainMaterial, this);
		myMeshComponent->SetMaterial(0, myMaterialInstanceDynamic);
	}
	else if(myMaterial)
	{
		myMeshComponent->SetMaterial(0, myMaterial);
	}
	if (aWorldConfig.CollisionEnabled)
	{
		myMeshComponent->CookCollisionNow();
	}
	
}

UMaterialInstanceDynamic* ACGWorldFace::GetMaterialInstanceDynamic()
{
	return myMaterialInstanceDynamic;
}
