#include "cashgenUE.h"
#include "ZoneManager.h"

AZoneManager::AZoneManager()
{
	PrimaryActorTick.bCanEverTick = true;
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	MyProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	SetupZone(20, 20, 100.0f);
}

void AZoneManager::SetupZone(int32 aX, int32 aY, float aUnitSize)
{
	gridSize = aUnitSize;
	worldGen = new WorldGenerator();
	worldGen->InitialiseTerrainGrid(aX, aY);
	worldGrid = worldGen->GetTerrainGrid();

	LoadTerrainGridAndGenerateMesh();
}

void AZoneManager::LoadTerrainGridAndGenerateMesh()
{
	for (int32 x = 0; x < worldGrid->Num(); ++x)
	{
		for (int32 y = 0; y < (*worldGrid)[x].blocks.Num(); ++y)
		{
			AddQuad(&(*worldGrid)[x].blocks[y], x, y);
		}
	}

	CreateSection();
}

void AZoneManager::AddQuad(ZoneBlock* block, int32 aX, int32 aY)
{
	int32 numTriangles = MyVertices.Num();
	MyVertices.Add(FVector(aX * gridSize - (gridSize*0.5), (aY * gridSize) - (gridSize*0.5), block->Height));
	MyVertices.Add(FVector(aX * gridSize - (gridSize*0.5), (aY * gridSize) + (gridSize*0.5), block->Height));
	MyVertices.Add(FVector((aX * gridSize) + (gridSize * 0.5), (aY * gridSize) - (gridSize*0.5), block->Height));

	MyVertices.Add(FVector((aX * gridSize) - (gridSize*0.5), (aY * gridSize) + (gridSize*0.5), block->Height));
	MyVertices.Add(FVector((aX * gridSize) + (gridSize*0.5), (aY * gridSize) + (gridSize*0.5), block->Height));
	MyVertices.Add(FVector((aX * gridSize) + (gridSize*0.5), (aY * gridSize) - (gridSize*0.5), block->Height));
	
	MyTriangles.Add(numTriangles);
	MyTriangles.Add(numTriangles + 1);
	MyTriangles.Add(numTriangles + 2);
	MyTriangles.Add(numTriangles + 3);
	MyTriangles.Add(numTriangles + 4);
	MyTriangles.Add(numTriangles + 5);

	MyNormals.Add(FVector(0, 0, 1));
	MyNormals.Add(FVector(0, 0, 1));
	MyNormals.Add(FVector(0, 0, 1));
	MyNormals.Add(FVector(0, 0, 1));
	MyNormals.Add(FVector(0, 0, 1));
	MyNormals.Add(FVector(0, 0, 1));

	MyUV0.Add(FVector2D(0, 0));
	MyUV0.Add(FVector2D(0, 10));
	MyUV0.Add(FVector2D(10, 0));
	MyUV0.Add(FVector2D(0, 10));
	MyUV0.Add(FVector2D(10, 10));
	MyUV0.Add(FVector2D(10, 0));

	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);

	/*MyTangents.Add(FProcMeshTangent(1, 1, 1));
	MyTangents.Add(FProcMeshTangent(1, 1, 1));
	MyTangents.Add(FProcMeshTangent(1, 1, 1));
	MyTangents.Add(FProcMeshTangent(1, 1, 1));
	MyTangents.Add(FProcMeshTangent(1, 1, 1));
	MyTangents.Add(FProcMeshTangent(1, 1, 1));*/
}

void AZoneManager::CreateSection()
{
	MyProcMesh->CreateMeshSection(1, MyVertices, MyTriangles, MyNormals, MyUV0, MyVertexColors, MyTangents, false);
	
	MyProcMesh->AttachTo(RootComponent);
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

}