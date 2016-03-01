#include "cashgenUE.h"
#include "ZoneManager.h"
#include "FZoneGeneratorWorker.h"

AZoneManager::AZoneManager()
{
	PrimaryActorTick.bCanEverTick = true;
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	MyProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
}

// Initial setup of the zone
void AZoneManager::SetupZone(Point aOffset, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, float aFloor, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed)
{
	MyOffset.x = aOffset.x;
	MyOffset.y = aOffset.y;
	MyConfig.XUnits = aX;
	MyConfig.YUnits = aY;
	MyConfig.UnitSize = aUnitSize;
	MyConfig.Floor = aFloor;
	MyConfig.Persistence = aPersistence;
	MyConfig.Frequency = aFrequency;
	MyConfig.Amplitude = aAmplitude;
	MyConfig.Octaves = aOctaves;
	MyConfig.RandomSeed = aRandomseed;

	MyMaterial = aMaterial;

	worldGen = new WorldGenerator();
	worldGen->InitialiseTerrainGrid(&MyZoneData, &MyHeightMap, aOffset, aX, aY, aFloor, aPersistence, aFrequency, aAmplitude, aOctaves, aRandomseed);
	LoadTerrainGridAndGenerateMesh(true);
}

void AZoneManager::RegenerateZone(Point aOffset)
{
	MyOffset.x = aOffset.x;
	MyOffset.y = aOffset.y;

	workerThreadCompleted = false;

	Thread = FRunnableThread::Create(new FZoneGeneratorWorker(this,
		&MyConfig,
		&MyOffset,
		&MyZoneData,
		&MyVertices,
		&MyTriangles,
		&MyNormals,
		&MyUV0,
		&MyVertexColors),
		TEXT("FZoneGeneratorWorker"),
		0, TPri_BelowNormal);



	// Wipe the zone data and heighmap (TODO: re-use instead of reallocate)
	//MyZoneData.Empty();
	//MyHeightMap.Empty();
	//worldGen->InitialiseTerrainGrid(&MyZoneData, &MyHeightMap, aOffset, aX, aY, aFloor, aPersistence, aFrequency, aAmplitude, aOctaves, aRandomseed);
	//LoadTerrainGridAndGenerateMesh(false);
}

void AZoneManager::LoadTerrainGridAndGenerateMesh(bool isNew)
{
	// If this is an update cycle, clearout the data structures
	if (!isNew)
	{
		MyVertices.Empty();
		MyTriangles.Empty();
		MyNormals.Empty();
		MyUV0.Empty();
		MyVertexColors.Empty();
	}
	// Generate the mesh data
	for (int32 x = 0; x < MyZoneData.Num() - 2; ++x)
	{
		for (int32 y = 0; y < MyZoneData[x].blocks.Num() - 2; ++y)
		{
			AddQuad(&MyZoneData[x + 1].blocks[y + 1], x, y);
		}
	}
	// Now tell the procmesh component to build/update
	if (isNew) {
		CreateSection();
	}
	else {
		UpdateSection();
	}
}

// This'll do for now, gives us face normals
FVector AZoneManager::CalcSurfaceNormalForTriangle(const int32 aStartTriangle)
{
	FVector v1 = MyVertices[aStartTriangle];
	FVector v2 = MyVertices[aStartTriangle + 1];
	FVector v3 = MyVertices[aStartTriangle + 2];

	FVector U = v2 - v1;
	FVector V = v3 - v1;

	return FVector::CrossProduct(V, U).GetSafeNormal();
}

// Builds a 2 tri square of mesh to cover the zoneblock
void AZoneManager::AddQuad(ZoneBlock* block, int32 aX, int32 aY)
{
	int32 numTriangles = MyVertices.Num();

	MyVertices.Add(FVector(aX * MyConfig.UnitSize + (MyConfig.UnitSize*0.5), (aY * MyConfig.UnitSize) - (MyConfig.UnitSize*0.5), block->bottomLeftCorner.height));
	MyVertices.Add(FVector(aX * MyConfig.UnitSize - (MyConfig.UnitSize*0.5), (aY * MyConfig.UnitSize) - (MyConfig.UnitSize*0.5), block->bottomRightCorner.height));
	MyVertices.Add(FVector((aX * MyConfig.UnitSize) + (MyConfig.UnitSize * 0.5), (aY * MyConfig.UnitSize) + (MyConfig.UnitSize*0.5), block->topLeftCorner.height));

	MyVertices.Add(FVector((aX * MyConfig.UnitSize) - (MyConfig.UnitSize*0.5), (aY * MyConfig.UnitSize) - (MyConfig.UnitSize*0.5), block->bottomRightCorner.height));
	MyVertices.Add(FVector((aX * MyConfig.UnitSize) - (MyConfig.UnitSize*0.5), (aY * MyConfig.UnitSize) + (MyConfig.UnitSize*0.5), block->topRightCorner.height));
	MyVertices.Add(FVector((aX * MyConfig.UnitSize) + (MyConfig.UnitSize*0.5), (aY * MyConfig.UnitSize) + (MyConfig.UnitSize*0.5), block->topLeftCorner.height));
	
	MyTriangles.Add(numTriangles);
	MyTriangles.Add(numTriangles + 1);
	MyTriangles.Add(numTriangles + 2);
	MyTriangles.Add(numTriangles + 3);
	MyTriangles.Add(numTriangles + 4);
	MyTriangles.Add(numTriangles + 5);

	FVector t1Normal = CalcSurfaceNormalForTriangle(numTriangles);
	FVector t2Normal = CalcSurfaceNormalForTriangle(numTriangles + 3);

	MyNormals.Add(t1Normal);
	MyNormals.Add(t1Normal);
	MyNormals.Add(t1Normal);
	MyNormals.Add(t2Normal);
	MyNormals.Add(t2Normal);
	MyNormals.Add(t2Normal);

	MyUV0.Add(FVector2D(0, 0));
	MyUV0.Add(FVector2D(0, 1));
	MyUV0.Add(FVector2D(1, 0));
	MyUV0.Add(FVector2D(0, 1));
	MyUV0.Add(FVector2D(1, 1));
	MyUV0.Add(FVector2D(1, 0));

	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
}

void AZoneManager::CreateSection()
{
	MyProcMesh->SetMaterial(0, MyMaterial);
	MyProcMesh->CreateMeshSection(0, MyVertices, MyTriangles, MyNormals, MyUV0, MyVertexColors, MyTangents, false);
	MyProcMesh->AttachTo(RootComponent);
}

void AZoneManager::UpdateSection()
{
	MyProcMesh->UpdateMeshSection(0, MyVertices, MyNormals, MyUV0, MyVertexColors, MyTangents);
}

AZoneManager::~AZoneManager()
{
	delete worldGen;
}

// Called when the game starts or when spawned
void AZoneManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AZoneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (workerThreadCompleted)
	{
		UpdateSection();
		delete Thread;
		Thread = NULL;
	}
}