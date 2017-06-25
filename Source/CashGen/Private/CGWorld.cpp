#include "cashgen.h"
#include "RuntimeMeshComponent.h"
#include "CGWorldGeneratorWorker.h"
#include "CGWorldFace.h"
#include "CGWorld.h"

bool ACGWorld::GetFreeMeshData(FCGWorldFaceJob& aJob)
{
	// No free mesh data
	if (MyFreeMeshData.Num() < 1)
	{
		return false;
	}
	else
	{
		FCGWorldMeshData* dataToUse;
		// Use the first free data set, there'll always be one, we checked!
		for (FCGWorldMeshData* data : MyFreeMeshData)
		{
			dataToUse = data;
			break;
		}
		// Add to the in use set
		MyInUseMeshData.Add(dataToUse);
		// Remove from the Free set
		MyFreeMeshData.Remove(dataToUse);

		// TODO: Fix this nasty allocation code, will do for now
		dataToUse->Allocate(aJob.SubDivisions);

		aJob.pMeshData = dataToUse;

		return true;
	}

	return false;
}

void ACGWorld::ReleaseMeshData(FCGWorldFaceJob& aJob)
{
	MyInUseMeshData.Remove(aJob.pMeshData);
	MyFreeMeshData.Add(aJob.pMeshData);
}

ACGWorld::ACGWorld(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
}

ACGWorld::~ACGWorld()
{
	if (MyWorkerThread != nullptr)
	{
		MyWorkerThread->Kill(true);
	}
}

void ACGWorld::BeginPlay()
{
	Super::BeginPlay();
	InitializeSphere(WorldConfig.Subdivisions, WorldConfig.Radius);

	FString threadName = "WorldWorkerThread";

	MyWorkerThread = FRunnableThread::Create(new FCGWorldGeneratorWorker(this, &WorldConfig, &GeometryJobs),
		*threadName,
		0, EThreadPriority::TPri_BelowNormal, FPlatformAffinity::GetNoAffinityMask());

	// Add some working data to begin with
	MyMeshData.Add(FCGWorldMeshData());
	MyFreeMeshData.Add(&MyMeshData[0]);

}

void ACGWorld::AddFace(ACGWorldFace* aFace)
{
	MyFaces.Add(aFace);
}

void ACGWorld::InitializeSphere(const int32 aDepth, const float aScale)
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
												WorldConfig.SubDivFaceLimit,
												WorldConfig.Radius,
												this,
												nullptr);
	}
	
}

void ACGWorld::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Check for pending jobs

	for (int i = 0; i < WorldConfig.MeshUpdatesPerFrame; i++)
	{
		FCGWorldFaceJob pendingJob;
		if (PendingJobs.Peek(pendingJob))
		{
			if (MyFreeMeshData.Num() > 0)
			{
				PendingJobs.Dequeue(pendingJob);
				GetFreeMeshData(pendingJob);
				GeometryJobs.Enqueue(pendingJob);
			}
		}
	}

	// Now check for geometry that is ready to be updated

	for (int i = 0; i < WorldConfig.MeshUpdatesPerFrame; i++)
	{
		FCGWorldFaceJob updateJob;
		if (UpdateJobs.Dequeue(updateJob))
		{
			updateJob.pFace->UpdateMesh(updateJob.pMeshData->Vertices, updateJob.pMeshData->Indices);

			ReleaseMeshData(updateJob);
		}
	}
}