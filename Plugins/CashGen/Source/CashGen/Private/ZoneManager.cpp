#include "cashgen.h"
#include "ZoneManager.h"
#include "WorldManager.h"
#include "MeshData.h"
#include "FZoneGeneratorWorker.h"

AZoneManager::AZoneManager()
{
	PrimaryActorTick.bCanEverTick = true;
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	MyProcMeshComponents.Add(0, CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh0")));
	MyProcMeshComponents.Add(1, CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh1")));
	MyProcMeshComponents.Add(2, CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh2")));
	MyProcMeshComponents.Add(3, CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh3")));

	//MyProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));




	this->SetActorEnableCollision(true);
	MyProcMeshComponents[0]->AttachTo(RootComponent);
	MyProcMeshComponents[1]->AttachTo(RootComponent);
	MyProcMeshComponents[2]->AttachTo(RootComponent);
	MyProcMeshComponents[3]->AttachTo(RootComponent);
	MyProcMeshComponents[0]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyProcMeshComponents[1]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyProcMeshComponents[2]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyProcMeshComponents[3]->BodyInstance.SetResponseToAllChannels(ECR_Block);

	currentlyDisplayedLOD = 10;
}

// Initial setup of the zone
void AZoneManager::SetupZone(int32 aZoneID, AWorldManager* aWorldManager, const Point aOffset, FZoneConfig aZoneConfig)
{
	MyOffset.x	= aOffset.x;
	MyOffset.y	= aOffset.y;
	MyConfig	= aZoneConfig;
	MyWorldManager = aWorldManager;
	MyZoneID = aZoneID;

	int32 compIndex = 0;
	for (int32 BiomeIndex = 0; BiomeIndex < MyConfig.BiomeConfig.Num(); ++BiomeIndex)
	{
		for (int32 MeshIndex = 0; MeshIndex < MyConfig.BiomeConfig[BiomeIndex].MeshConfigs.Num(); ++MeshIndex)
		{
			FString compString = FString::FromInt(MyOffset.x) + FString::FromInt(MyOffset.y) + FString::FromInt(MeshIndex) + FString::FromInt(BiomeIndex);
			FName compName = FName(*compString);
			MyInstancedMeshComponents.Add(ConstructObject<UInstancedStaticMeshComponent>(UInstancedStaticMeshComponent::StaticClass(), this, compName));
			MyInstancedMeshComponents[compIndex]->RegisterComponent();
			MyInstancedMeshComponents[compIndex]->SetStaticMesh(MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].Mesh);
			MyInstancedMeshComponents[compIndex]->bCastDynamicShadow = true;
			MyInstancedMeshComponents[compIndex]->CastShadow = true;
			MyInstancedMeshComponents[compIndex]->SetHiddenInGame(false);
			MyInstancedMeshComponents[compIndex]->SetMobility(EComponentMobility::Movable);

			MyInstancedMeshComponents[compIndex]->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			MyInstancedMeshComponents[compIndex]->BodyInstance.SetObjectType(ECC_WorldDynamic);
			MyInstancedMeshComponents[compIndex]->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
			MyInstancedMeshComponents[compIndex]->BodyInstance.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
			MyInstancedMeshComponents[compIndex]->BodyInstance.SetResponseToChannel(ECC_Terrain, ECR_Ignore);
			MyInstancedMeshComponents[compIndex]->BodyInstance.SetResponseToChannel(ECC_Pawn, MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].PawnCollisionResponse);
			MyInstancedMeshComponents[compIndex]->BodyInstance.SetResponseToChannel(ECC_WorldDynamic, ECR_Block);

			++compIndex;
		}
	}

	MyBlocksToSpawnFoliageOn = MyConfig.XUnits * MyConfig.YUnits;
}

void AZoneManager::PopulateZoneData(const uint8 aLOD)
{

	MyLODZoneData.Add(aLOD, TArray<GridRow>());
	int32 exX = aLOD == 0 ? MyConfig.XUnits + 2 : (MyConfig.XUnits / (FMath::Pow(2, aLOD))) + 2;
	int32 exY = aLOD == 0 ? MyConfig.YUnits + 2 : (MyConfig.YUnits / (FMath::Pow(2, aLOD))) + 2;

	MyLODZoneData[aLOD].Reserve(exX);
	for (int x = 0; x < exX; ++x)
	{
		GridRow row;
		row.blocks.Reserve(exY);
		for (int y = 0; y < exY; ++y)
		{
			row.blocks.Add(ZoneBlock(0.0f, FColor::Cyan, x, y));
		}
		MyLODZoneData[aLOD].Add(row);
	}
}

void AZoneManager::PopulateMeshData(const uint8 aLOD)
{
	int32 exX = aLOD == 0 ? MyConfig.XUnits + 2 : (MyConfig.XUnits / (FMath::Pow(2, aLOD))) + 2;
	int32 exY = aLOD == 0 ? MyConfig.YUnits + 2 : (MyConfig.YUnits / (FMath::Pow(2, aLOD))) + 2;

	MyLODMeshData.Add(aLOD, FMeshData());
	for (int32 i = 0; i < (exX + 1) * (exY + 1); ++i)
	{
		MyLODMeshData[aLOD].MyVertices.Add(FVector(i * 1.0f, i * 1.0f, 0.0f));
		MyLODMeshData[aLOD].MyUV0.Add(FVector2D(0.0f, 0.0f));
		MyLODMeshData[aLOD].MyVertexColors.Add(FColor::Black);
	}

	for (int32 i = 0; i < exX * exY * 6; ++i)
	{
		MyLODMeshData[aLOD].MyTriangles.Add(i);
	}

	CalculateTriangles(aLOD);
}


void AZoneManager::ClearAllInstancedMeshes()
{
	int32 compIndex = 0;
	for (int32 BiomeIndex = 0; BiomeIndex < MyConfig.BiomeConfig.Num(); ++BiomeIndex)
	{
		for (int32 MeshIndex = 0; MeshIndex < MyConfig.BiomeConfig[BiomeIndex].MeshConfigs.Num(); ++MeshIndex)
		{
			MyInstancedMeshComponents[compIndex]->ClearInstances();

			++compIndex;
		}
	}
}

void AZoneManager::CalculateTriangles(const uint8 aLOD)
{
	int32 triCounter = 0;
	int32 thisX, thisY;
	int32 lodX, lodY;

	lodX = aLOD == 0 ? MyConfig.XUnits : MyConfig.XUnits / (FMath::Pow(2, aLOD));
	lodY = aLOD == 0 ? MyConfig.YUnits : MyConfig.YUnits / (FMath::Pow(2, aLOD));

	for (int32 y = 0; y < (MyLODZoneData[aLOD])[0].blocks.Num() - 2; ++y)
	{
		for (int32 x = 0; x < MyLODZoneData[aLOD].Num() - 2; ++x)
		{

			thisX = MyLODZoneData[aLOD][x + 1].blocks[y + 1].MyX - 1;
			thisY = MyLODZoneData[aLOD][x + 1].blocks[y + 1].MyY - 1;
			//TR
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = thisX + ((thisY + 1) * (lodX + 1));
			triCounter++;
			//BL
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = (thisX + 1) + (thisY * (lodX + 1));
			triCounter++;
			//BR
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = thisX + (thisY * (lodX + 1));
			triCounter++;

			//BL
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = (thisX + 1) + (thisY * (lodX + 1));
			triCounter++;
			//TR
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = thisX + ((thisY + 1) * (lodX + 1));
			triCounter++;
			// TL
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = (thisX + 1) + ((thisY + 1) * (lodX + 1));
			triCounter++;

			//TR
			MyLODMeshData[aLOD].MyUV0[thisX + ((thisY + 1) * (lodX + 1))] = FVector2D(thisX * 1.0f, (thisY+1.0f) * 1.0f);
			//BR
			MyLODMeshData[aLOD].MyUV0[thisX + (thisY * (lodX + 1))] = FVector2D(thisX * 1.0f, thisY * 1.0f);
			//BL
			MyLODMeshData[aLOD].MyUV0[(thisX + 1) + (thisY * (lodX + 1))] = FVector2D((thisX + 1.0f) * 1.0f, thisY * 1.0f);
			//TL
			MyLODMeshData[aLOD].MyUV0[(thisX + 1) + ((thisY + 1) * (lodX + 1))] = FVector2D((thisX +1.0f)* 1.0f, (thisY+1.0f) * 1.0f);

		}
	}
}

void AZoneManager::InitialiseBlockPointers(const uint8 aLOD)
{
	int32 exX = aLOD == 0 ? MyConfig.XUnits + 2 : (MyConfig.XUnits / (FMath::Pow(2, aLOD))) + 2;
	int32 exY = aLOD == 0 ? MyConfig.YUnits + 2 : (MyConfig.YUnits / (FMath::Pow(2, aLOD))) + 2;

	// Now set the LRUD pointers
	for (int x = 0; x < exX; ++x)
	{
		for (int y = 0; y < exY; ++y)
		{
			// Bottom right case
			if (x == 0 && y == 0)
			{
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// top right case
			if (x == 0 && y == exY - 1)
			{
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// top left case
			if (x == exX - 1 && y == exY - 1)
			{
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// bottom left case
			if (x == exX - 1 && y == 0)
			{
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}

			// Set right edge pointers
			if (x == 0 && y > 0 && y < exY - 1)
			{
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = nullptr;

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// Set left edge pointers
			if (x == exX - 1 && y > 0 && y < exY - 1) {
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y - 1];
			}
			// Set bottom edge pointers
			if (y == 0 && x > 0 && x < exX - 1) {
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = nullptr;

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = nullptr;
			}
			// Set top edge pointers
			if (y == exY - 1 && x > 0 && x < exX - 1) {
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = nullptr;
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y - 1];
			}

			// Normal cases :
			if (x > 0 && x < exX - 1 && y > 0 && y < exY - 1) {
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y + 1];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].topRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];

				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.topRightBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x + 1].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomLeftCorner.bottomRightBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];

				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topLeftBlock = &MyLODZoneData[aLOD][x].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.topRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomLeftBlock = &MyLODZoneData[aLOD][x].blocks[y - 1];
				MyLODZoneData[aLOD][x].blocks[y].bottomRightCorner.bottomRightBlock = &MyLODZoneData[aLOD][x - 1].blocks[y - 1];
			}
		}
	}
}

void AZoneManager::RegenerateZone(const uint8 aLOD, const bool isInPlaceLODUpdate)
{
	if (!isInPlaceLODUpdate) {
		for (uint8 i = 0; i < MyProcMeshComponents.Num(); ++i)
		{
			MyProcMeshComponents[i]->SetMeshSectionVisible(0, false);
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(5, 3.0f, FColor::Red, TEXT("In place update!"));
	}

	if (currentlyDisplayedLOD == 0)
	{
		ClearAllInstancedMeshes();
	}

	currentlyDisplayedLOD = aLOD;

	if (aLOD != 10)
	{
		FString threadName = "ZoneWorker" + FString::FromInt(MyOffset.x) + FString::FromInt(MyOffset.y) + FString::FromInt(aLOD);

		// If the data structures for this LOD aren't ready, build them.
		if (!MyLODMeshData.Contains(aLOD))
		{
			MyLODMeshStatus.Add(aLOD, eLODStatus::BUILDING_REQUIRES_CREATE);
			PopulateZoneData(aLOD);
			PopulateMeshData(aLOD);
			InitialiseBlockPointers(aLOD);
			MyProcMeshComponents[aLOD]->SetMaterial(0, MyConfig.TerrainMaterial);
		}
		else
		{
			MyLODMeshStatus[aLOD] = eLODStatus::BUILDING;
		}

		Thread = FRunnableThread::Create(new FZoneGeneratorWorker(this,
			&MyConfig,
			&MyOffset,
			&MyLODMeshStatus,
			aLOD,
			&MyLODZoneData[aLOD],

			&MyLODMeshData[aLOD].MyVertices,
			&MyLODMeshData[aLOD].MyTriangles,
			&MyLODMeshData[aLOD].MyNormals,
			&MyLODMeshData[aLOD].MyUV0,
			&MyLODMeshData[aLOD].MyVertexColors,
			&MyLODMeshData[aLOD].MyTangents),
			*threadName,
			0, TPri_BelowNormal);
	}







	SetActorLocation(FVector(MyConfig.XUnits * MyConfig.UnitSize * MyOffset.x, MyConfig.YUnits * MyConfig.UnitSize * MyOffset.y, 0.0f));
}


void AZoneManager::UpdateMesh(const uint8 aLOD)
{


	// Create the mesh sections if they haven't been done already
	if (MyLODMeshStatus[aLOD] == eLODStatus::DRAWING_REQUIRES_CREATE)
	{
		MyProcMeshComponents[aLOD]->CreateMeshSection(0, MyLODMeshData[aLOD].MyVertices, MyLODMeshData[aLOD].MyTriangles, MyLODMeshData[aLOD].MyNormals, MyLODMeshData[aLOD].MyUV0, MyLODMeshData[aLOD].MyVertexColors, MyLODMeshData[aLOD].MyTangents, aLOD == 0);
		MyLODMeshStatus[aLOD] = IDLE;
	}
	// Or just update them
	else if (MyLODMeshStatus[aLOD] == eLODStatus::DRAWING)
	{
		MyProcMeshComponents[aLOD]->UpdateMeshSection(0, MyLODMeshData[aLOD].MyVertices, MyLODMeshData[aLOD].MyNormals, MyLODMeshData[aLOD].MyUV0, MyLODMeshData[aLOD].MyVertexColors, MyLODMeshData[aLOD].MyTangents);
		MyLODMeshStatus[aLOD] = IDLE;

		if (aLOD == 0)
		{
			for (auto& InstancedStaticMeshComponent : MyInstancedMeshComponents)
			{
				InstancedStaticMeshComponent->ClearInstances();
			}
			MyBlocksToSpawnFoliageOn = MyConfig.XUnits * MyConfig.YUnits;
		}
	}

	// Now show the new section
	for (uint8 i = 0; i < MyProcMeshComponents.Num(); ++i)
	{
		if (i == aLOD) {
			MyProcMeshComponents[i]->SetMeshSectionVisible(0, true);
		}
		else {
			MyProcMeshComponents[i]->SetMeshSectionVisible(0, false);
		}
	}

}

bool AZoneManager::SpawnTreesAtIndex(int32* aIndex)
{
	int32 counter_Temp = (MyConfig.XUnits * MyConfig.YUnits) - (*aIndex);

	bool hasSpawnedOnThisBlock = false;

	int32 blockX, blockY;
	blockX = (counter_Temp % (MyConfig.YUnits)) + 1;
	blockY = (counter_Temp / (MyConfig.YUnits)) + 1;

	(*aIndex)--;

	int32 compIndex = 0;

	for (int32 BiomeIndex = 0; BiomeIndex < MyConfig.BiomeConfig.Num(); ++BiomeIndex)
	{
		for (int32 MeshIndex = 0; MeshIndex < MyConfig.BiomeConfig[BiomeIndex].MeshConfigs.Num(); ++MeshIndex)
		{
			if (MyLODZoneData[0][blockX].blocks[blockY].Biome == BiomeIndex)
			{
				for (int32 densityMultiplier = 0; densityMultiplier < MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].DensityMultiplier; ++densityMultiplier)
				{
					if (MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].DensityMultiplier > 0.0f
							&& MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].DensityMultiplier < 1.0f
							&& MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].DensityMultiplier < FMath::FRandRange(0.0, 1.0))
					{
						hasSpawnedOnThisBlock = true;
						break;
					}

					FVector startPos = FVector(((MyOffset.x) * MyConfig.XUnits * MyConfig.UnitSize) + ((blockX)* MyConfig.UnitSize) + FMath::FRandRange(0.0f, MyConfig.UnitSize),
						(MyOffset.y * MyConfig.YUnits * MyConfig.UnitSize) + ((blockY)* MyConfig.UnitSize) + FMath::FRandRange(0.0f, MyConfig.UnitSize), 50000.0f);

					FVector spawnPoint = FVector(0.0f, 0.0f, 0.0f);
					FVector normalVector = FVector(0.0f);
					if (GetGodCastHitPos(startPos, &spawnPoint, &normalVector))
					{
						FRotator rotation = FRotator(0.0f, FMath::FRandRange(-90.f, 90.0f), 0.0f);

						if (MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].AlignWithPlane)
						{
							rotation += normalVector.Rotation() + FRotator(-90.0f, 0.0f, 0.0f);
						}



						MyInstancedMeshComponents[compIndex]->AddInstance(FTransform(rotation, spawnPoint, FVector(1.0f) * FMath::RandRange(MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].ScaleMin, MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].ScaleMax)));
						hasSpawnedOnThisBlock = true;
					}
				}
			}

			++compIndex;

		}
	}

	return hasSpawnedOnThisBlock;
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

	for (auto& lod : MyLODMeshStatus)
	{
		if (lod.Value == eLODStatus::READY_TO_DRAW)
		{
			lod.Value = eLODStatus::PENDING_DRAW;

			// Thread finished, give it back to the queue
			MyWorldManager->MyAvailableThreads.Enqueue(1);

			//UpdateMesh(lod.Key);
			MyWorldManager->MyRenderQueue.Enqueue(FZoneJob(MyZoneID, lod.Key, false));
			delete Thread;
			Thread = NULL;
		}
		else if (lod.Value == eLODStatus::READY_TO_DRAW_REQUIRES_CREATE)
		{
			lod.Value = eLODStatus::PENDING_DRAW_REQUIRES_CREATE;

			// Thread finished, give it back to the queue
			MyWorldManager->MyAvailableThreads.Enqueue(1);
			MyWorldManager->MyRenderQueue.Enqueue(FZoneJob(MyZoneID, lod.Key, false));
			delete Thread;
			Thread = NULL;
		}

	}

	if (MyLODMeshStatus.Contains(0) && MyLODMeshStatus[0] == eLODStatus::IDLE)
	{
		for (int i = 0; i < 50; ++i)
		{
			while (MyBlocksToSpawnFoliageOn > 0 && !SpawnTreesAtIndex(&MyBlocksToSpawnFoliageOn));
		}
	}


}

void AZoneManager::CreateWaterPlane(const float aWaterHeight)
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

	//MyProcMesh->CreateMeshSection(1, waterVerts, waterTriangles, waterNormals, waterUVs, waterVertColors, waterTangents, true);
}

// Raycasts vertically down from the given point and returns the point it strikes the terrain
bool AZoneManager::GetGodCastHitPos(const FVector aVectorToStart, FVector* aHitPos, FVector* aNormalVector)
{
	const FName TraceTag("MyTraceTag");
	FCollisionQueryParams MyTraceParams = FCollisionQueryParams(FName(TEXT("TreeTrace")), true);
	MyTraceParams.bTraceComplex = false;
	MyTraceParams.bTraceAsyncScene = true;
	MyTraceParams.bReturnPhysicalMaterial = false;
	//MyTraceParams.TraceTag = TraceTag;
	//GetWorld()->DebugDrawTraceTag = TraceTag;

	FCollisionResponseParams MyResponseParams = FCollisionResponseParams();

	FHitResult MyHitResult(ForceInit);

	FVector MyCastDirection = FVector(0.0f, 0.0f, -1.0f);

	if (GetWorld()->LineTraceSingleByChannel(MyHitResult, aVectorToStart, aVectorToStart + (MyCastDirection * 900000.0f), ECC_Terrain, MyTraceParams, MyResponseParams))
	{
		AActor* hitActor = MyHitResult.GetActor();
		if (hitActor)
		{
			(*aNormalVector) = MyHitResult.Normal;
			(*aHitPos) = MyHitResult.ImpactPoint;
			return true;
		}
	}

	return false;




}

FVector AZoneManager::GetCentrePos()
{
	return  FVector(MyOffset.x * MyConfig.XUnits * MyConfig.UnitSize, MyOffset.y * MyConfig.YUnits * MyConfig.UnitSize, 0.0f);
}