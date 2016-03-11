#include "cashgenUE.h"
#include "ZoneManager.h"
#include "WorldManager.h"
#include "FZoneGeneratorWorker.h"


AZoneManager::AZoneManager()
{
	PrimaryActorTick.bCanEverTick = true;
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	MyProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
}

// Initial setup of the zone
void AZoneManager::SetupZone(AWorldManager* aWorldManager, Point aOffset, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, UMaterial* aWaterMaterial, float aFloorDepth, float aFloorHeight, float aWaterHeight, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed)
{
	MyOffset.x = aOffset.x;
	MyOffset.y = aOffset.y;
	MyConfig.XUnits = aX;
	MyConfig.YUnits = aY;
	MyConfig.UnitSize = aUnitSize;
	MyConfig.FloorDepth = aFloorDepth;
	MyConfig.FloorHeight = aFloorHeight;
	MyConfig.Persistence = aPersistence;
	MyConfig.Frequency = aFrequency;
	MyConfig.Amplitude = aAmplitude;
	MyConfig.Octaves = aOctaves;
	MyConfig.RandomSeed = aRandomseed;

	MyWorldManager = aWorldManager;

	MyMaterial = aMaterial;
	MyWaterMaterial = aWaterMaterial;

	PopulateDataStructures();
	InitialiseBlockPointers();
	//CreateSection();
	CreateWaterPlane(aWaterHeight);

	hasCreatedMesh = false;
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
	
	for (int32 i = 0; i < (MyConfig.XUnits + 1) * (MyConfig.YUnits + 1); ++i)
	{
		MyVertices.Add(FVector(i * 1.0f, i * 1.0f, 0.0f));
		MyUV0.Add(FVector2D(0.0f, 0.0f));
		MyVertexColors.Add(FColor::Black);
	}

	for (int32 i = 0; i < (MyConfig.XUnits) * (MyConfig.YUnits) * 6; ++i)
	{
		MyTriangles.Add(i);
	}

	CalculateTriangles();
}

void AZoneManager::CalculateUV0()
{

}

void AZoneManager::CalculateTriangles()
{
	int32 triCounter = 0;
	int32 thisX, thisY;
	
	for (int32 y = 0; y < (MyZoneData)[0].blocks.Num() - 2; ++y)
	{
		for (int32 x = 0; x < MyZoneData.Num() - 2; ++x)
		{
			
			thisX = MyZoneData[x + 1].blocks[y + 1].MyX - 1;
			thisY = MyZoneData[x + 1].blocks[y + 1].MyY - 1;
			//TR
			(MyTriangles)[triCounter] = thisX + ((thisY + 1) * (MyConfig.XUnits + 1));
			triCounter++;
			//BL
			(MyTriangles)[triCounter] = (thisX + 1) + (thisY * (MyConfig.XUnits + 1));
			triCounter++;
			//BR
			(MyTriangles)[triCounter] = thisX + (thisY * (MyConfig.XUnits + 1));
			triCounter++;
			
			//BL
			(MyTriangles)[triCounter] = (thisX + 1) + (thisY * (MyConfig.XUnits + 1));
			triCounter++;
			//TR
			(MyTriangles)[triCounter] = thisX + ((thisY + 1) * (MyConfig.XUnits + 1));
			triCounter++;
			// TL
			(MyTriangles)[triCounter] = (thisX + 1) + ((thisY + 1) * (MyConfig.XUnits + 1));
			triCounter++;

			//TR
			MyUV0[thisX + ((thisY + 1) * (MyConfig.XUnits + 1))] = FVector2D(thisX * 100.0f, (thisY+1.0f) * 100.0f);
			//BR
			MyUV0[thisX + (thisY * (MyConfig.XUnits + 1))] = FVector2D(thisX * 100.0f, thisY * 100.0f);
			//BL
			MyUV0[(thisX + 1) + (thisY * (MyConfig.XUnits + 1))] = FVector2D((thisX + 1.0f) * 100.0f, thisY * 100.0f);
			//TL
			MyUV0[(thisX + 1) + ((thisY + 1) * (MyConfig.XUnits + 1))] = FVector2D((thisX +1.0f)* 100.0f, (thisY+1.0f) * 100.0f);
			
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
		&MyVertexColors,
		&MyTangents),
		*threadName,
		0, TPri_BelowNormal);
}


void AZoneManager::UpdateSection()
{
	if (!hasCreatedMesh)
	{
		this->SetActorEnableCollision(true);

		MyProcMesh->SetMaterial(0, MyMaterial);
		MyProcMesh->SetMaterial(1, MyWaterMaterial);
		MyProcMesh->CreateMeshSection(0, MyVertices, MyTriangles, MyNormals, MyUV0, MyVertexColors, MyTangents, true);
		MyProcMesh->AttachTo(RootComponent);

		hasCreatedMesh = true;
	} 
	else
	{
		MyProcMesh->UpdateMeshSection(0, MyVertices, MyNormals, MyUV0, MyVertexColors, MyTangents);
		MyProcMesh->WakeRigidBody();
	}

}

AZoneManager::~AZoneManager()
{

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
		MyWorldManager->MyNumThreads--;
		workerThreadCompleted = false;
		UpdateSection();
		SetActorLocation(FVector(MyConfig.XUnits * MyConfig.UnitSize * MyOffset.x, MyConfig.YUnits * MyConfig.UnitSize * MyOffset.y, 0.0f));
		delete Thread;
		Thread = NULL;
	}


	
}

void AZoneManager::CreateWaterPlane(float aWaterHeight)
{
	float waterHeight = aWaterHeight;
	TArray<FVector> waterVerts;
	TArray<int32> waterTriangles;
	TArray<FVector> waterNormals;
	TArray<FVector2D> waterUVs;
	TArray<FColor> waterVertColors;
	TArray<FProcMeshTangent> waterTangents;

	waterVerts.Add(FVector(MyConfig.XUnits * MyConfig.UnitSize, 1.0f * MyConfig.YUnits * MyConfig.UnitSize, waterHeight));//UL
	waterVerts.Add(FVector(MyConfig.XUnits * MyConfig.UnitSize, 0.0f, waterHeight));//DL
	waterVerts.Add(FVector(0.0f, 0.0f, waterHeight)); //DR

	waterVerts.Add(FVector(MyConfig.XUnits * MyConfig.UnitSize, 1.0f * MyConfig.YUnits * MyConfig.UnitSize, waterHeight)); // UL
	waterVerts.Add(FVector(0.0f, 0.0f, waterHeight)); //DR
	waterVerts.Add(FVector(0.0f, 1.0f * MyConfig.YUnits * MyConfig.UnitSize, waterHeight)); //UR

	waterTriangles.Add(0);
	waterTriangles.Add(1);
	waterTriangles.Add(2);
	waterTriangles.Add(3);
	waterTriangles.Add(4);
	waterTriangles.Add(5);

	waterNormals.Add(FVector(0.0f, 0.0f, 1.0f));
	waterNormals.Add(FVector(0.0f, 0.0f, 1.0f));
	waterNormals.Add(FVector(0.0f, 0.0f, 1.0f));
	waterNormals.Add(FVector(0.0f, 0.0f, 1.0f));
	waterNormals.Add(FVector(0.0f, 0.0f, 1.0f));
	waterNormals.Add(FVector(0.0f, 0.0f, 1.0f));

	waterUVs.Add(FVector2D(-50, 50));
	waterUVs.Add(FVector2D(-50, 0));
	waterUVs.Add(FVector2D(0, 0));
	waterUVs.Add(FVector2D(-50, 50));
	waterUVs.Add(FVector2D(0, 0));
	waterUVs.Add(FVector2D(0, 50));

	MyProcMesh->CreateMeshSection(1, waterVerts, waterTriangles, waterNormals, waterUVs, waterVertColors, waterTangents, true);
}