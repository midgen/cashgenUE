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

	PopulateDataStructures();
	InitialiseBlockPointers();
	CreateSection();

	//worldGen = new WorldGenerator();
	//worldGen->InitialiseTerrainGrid(&MyZoneData, &MyHeightMap, aOffset, aX, aY, aFloor, aPersistence, aFrequency, aAmplitude, aOctaves, aRandomseed);
	//LoadTerrainGridAndGenerateMesh(true);
}

void AZoneManager::PopulateDataStructures()
{
	int32 exX = MyConfig.XUnits + 2;
	int32 exY = MyConfig.YUnits + 2;

	MyZoneData.Reserve(exX);
	for (int x = 0; x < exX; ++x)
	{
		GridRow row;
		row.blocks.Reserve(exY);
		for (int y = 0; y < exY; ++y)
		{
			row.blocks.Add(ZoneBlock(0.0f, FColor::Cyan, x, y));
		}
		MyZoneData.Add(row);
	}

	for (int32 x = 0; x < MyZoneData.Num() - 2; ++x)
	{
		for (int32 y = 0; y < MyZoneData[x].blocks.Num() - 2; ++y)
		{
			AddQuad(&MyZoneData[x + 1].blocks[y + 1], x, y);
		}
	}
}

void AZoneManager::InitialiseBlockPointers()
{
	int32 exX = MyConfig.XUnits + 2;
	int32 exY = MyConfig.YUnits + 2;
	// Now set the LRUD pointers
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			// Bottom right case
			if (x == 0 && y == 0)
			{
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// top right case
			if (x == 0 && y == exY - 1)
			{
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y - 1];

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// top left case
			if (x == exX - 1 && y == exY - 1)
			{
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y];

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y - 1];

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// bottom left case
			if (x == exX - 1 && y == 0)
			{
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y];

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}

			// Set right edge pointers
			if (x == 0 && y > 0 && y < exY - 1)
			{
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y + 1];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y - 1];

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// Set left edge pointers
			if (x == exX - 1 && y > 0 && y < exY - 1) {
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y];

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y - 1];

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y - 1];
			}
			// Set bottom edge pointers
			if (y == 0 && x > 0 && x < exX - 1) {
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y];

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// Set top edge pointers
			if (y == exY - 1 && x > 0 && x < exX - 1) {
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y];

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y - 1];

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y - 1];
			}

			// Normal cases :
			if (x > 0 && x < exX - 1 && y > 0 && y < exY - 1) {
				MyZoneData[x].blocks[y].topLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.topRightBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].topLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y];

				MyZoneData[x].blocks[y].topRightCorner.topLeftBlock = &MyZoneData[x].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y + 1];
				MyZoneData[x].blocks[y].topRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].topRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y];

				MyZoneData[x].blocks[y].bottomLeftCorner.topLeftBlock = &MyZoneData[x + 1].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.topRightBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyZoneData[x + 1].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyZoneData[x].blocks[y - 1];

				MyZoneData[x].blocks[y].bottomRightCorner.topLeftBlock = &MyZoneData[x].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.topRightBlock = &MyZoneData[x - 1].blocks[y];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyZoneData[x].blocks[y - 1];
				MyZoneData[x].blocks[y].bottomRightCorner.bottomRightBlock = &MyZoneData[x - 1].blocks[y - 1];
			}
		}
	}
}

void AZoneManager::RegenerateZone()
{
	FString threadName = "ZoneWorker" + MyOffset.x + MyOffset.y;
	
	

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
		*threadName,
		0, TPri_BelowNormal);



	// Wipe the zone data and heighmap (TODO: re-use instead of reallocate)
	//MyZoneData.Empty();
	//MyHeightMap.Empty();
	//worldGen->InitialiseTerrainGrid(&MyZoneData, &MyHeightMap, aOffset, aX, aY, aFloor, aPersistence, aFrequency, aAmplitude, aOctaves, aRandomseed);
	//LoadTerrainGridAndGenerateMesh(false);
}

void AZoneManager::LoadTerrainGridAndGenerateMesh(bool isNew)
{

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
		
		workerThreadCompleted = false;
		UpdateSection();
		SetActorLocation(FVector(MyConfig.XUnits * MyConfig.UnitSize * MyOffset.x, MyConfig.YUnits * MyConfig.UnitSize * MyOffset.y, -10000.0f));
		delete Thread;
		Thread = NULL;
	}

	if (GetActorLocation().Z < 0.0f)
	{
		FVector newLoc = GetActorLocation() + FVector(0.0f, 0.0f, 10000.0f * DeltaTime);
		if (newLoc.Z > 0.0f)
		{
			newLoc.Z = 0.0f;
		}

		SetActorLocation(newLoc);
	}
	
}