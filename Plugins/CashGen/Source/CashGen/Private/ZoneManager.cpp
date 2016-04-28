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

	// A proc mesh component for each of the LOD levels - will make this configurable
	MyProcMeshComponents.Add(0, CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh0")));
	MyProcMeshComponents.Add(1, CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh1")));
	MyProcMeshComponents.Add(2, CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh2")));
	MyProcMeshComponents.Add(3, CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh3")));

	this->SetActorEnableCollision(true);
	MyProcMeshComponents[0]->AttachTo(RootComponent);
	MyProcMeshComponents[1]->AttachTo(RootComponent);
	MyProcMeshComponents[2]->AttachTo(RootComponent);
	MyProcMeshComponents[3]->AttachTo(RootComponent);
	MyProcMeshComponents[0]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyProcMeshComponents[1]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyProcMeshComponents[2]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyProcMeshComponents[3]->BodyInstance.SetResponseToAllChannels(ECR_Block);

	// LOD 10 = do not render
	currentlyDisplayedLOD = 10;
}

// Initial setup of the zone
void AZoneManager::SetupZone(int32 aZoneID, AWorldManager* aWorldManager, const Point aOffset, FZoneConfig aZoneConfig)
{
	// The world offset of this zone
	MyOffset.x	= aOffset.x;
	MyOffset.y	= aOffset.y;
	// Config, manager pointers etc.
	MyConfig	= aZoneConfig;
	MyWorldManager = aWorldManager;
	MyZoneID = aZoneID;

	// This section creates an instanced mesh component for each one specified in the Biome configuration
	int32 compIndex = 0;
	for (int32 BiomeIndex = 0; BiomeIndex < MyConfig.BiomeConfig.Num(); ++BiomeIndex)
	{
		for (int32 MeshIndex = 0; MeshIndex < MyConfig.BiomeConfig[BiomeIndex].MeshConfigs.Num(); ++MeshIndex)
		{
			FString compString = FString::FromInt(MyOffset.x) + FString::FromInt(MyOffset.y) + FString::FromInt(MeshIndex) + FString::FromInt(BiomeIndex);
			FName compName = FName(*compString);
			MyInstancedMeshComponents.Add(NewObject<UInstancedStaticMeshComponent>(this, UInstancedStaticMeshComponent::StaticClass()));
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
	
}

// Populates the mesh data structures for a given LOD
void AZoneManager::PopulateMeshData(const uint8 aLOD)
{
	int32 numXVerts = aLOD == 0 ? MyConfig.XUnits + 1 : (MyConfig.XUnits / (FMath::Pow(2, aLOD))) + 1;
	int32 numYVerts = aLOD == 0 ? MyConfig.YUnits + 1 : (MyConfig.YUnits / (FMath::Pow(2, aLOD))) + 1;

	MyLODMeshData.Add(aLOD, FMeshData());
	// Generate the per vertex data sets
	for (int32 i = 0; i < (numXVerts * numYVerts); ++i)
	{
		MyLODMeshData[aLOD].MyVertices.Add(FVector(i * 1.0f, i * 1.0f, 0.0f));
		MyLODMeshData[aLOD].MyNormals.Add(FVector(0.0f, 0.0f, 1.0f));
		MyLODMeshData[aLOD].MyUV0.Add(FVector2D(0.0f, 0.0f));
		MyLODMeshData[aLOD].MyVertexColors.Add(FColor::Black);
	}

	// Heightmap needs to be larger than the mesh
	for (int32 i = 0; i < (numXVerts + 2) * (numYVerts + 2); ++i)
	{
		MyLODMeshData[aLOD].MyHeightMap.Add(FVector(0.0f));
	}

	// Triangle indexes
	for (int32 i = 0; i < (numXVerts - 1) * (numYVerts - 1) * 6; ++i)
	{
		MyLODMeshData[aLOD].MyTriangles.Add(i);
	}

	CalculateTriangles(aLOD);
}

// Trash all the instanced mesh instances on this zone (expensive, optimising to be done)
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

// Calculate the triangles and UVs for this LOD
// TODO: Create proper UVs
void AZoneManager::CalculateTriangles(const uint8 aLOD)
{
	int32 triCounter = 0;
	int32 thisX, thisY;
	int32 rowLength;

	rowLength = aLOD == 0 ? MyConfig.XUnits + 1 : (MyConfig.XUnits / (FMath::Pow(2, aLOD)) + 1);
	
	int32 exX = aLOD == 0 ? MyConfig.XUnits : (MyConfig.XUnits / (FMath::Pow(2, aLOD)));
	int32 exY = aLOD == 0 ? MyConfig.YUnits : (MyConfig.YUnits / (FMath::Pow(2, aLOD)));

	for (int32 y = 0; y < exY ; ++y)
	{
		for (int32 x = 0; x < exX; ++x)
		{

			thisX = x;
			thisY = y;
			//TR
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = thisX + ((thisY + 1) * (rowLength));
			triCounter++;
			//BL
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = (thisX + 1) + (thisY * (rowLength));
			triCounter++;
			//BR
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = thisX + (thisY * (rowLength));
			triCounter++;

			//BL
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = (thisX + 1) + (thisY * (rowLength));
			triCounter++;
			//TR
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = thisX + ((thisY + 1) * (rowLength));
			triCounter++;
			// TL
			(MyLODMeshData[aLOD].MyTriangles)[triCounter] = (thisX + 1) + ((thisY + 1) * (rowLength));
			triCounter++;

			//TR
			MyLODMeshData[aLOD].MyUV0[thisX + ((thisY + 1) * (rowLength))] = FVector2D(thisX * 1.0f, (thisY+1.0f) * 1.0f);
			//BR
			MyLODMeshData[aLOD].MyUV0[thisX + (thisY * (rowLength))] = FVector2D(thisX * 1.0f, thisY * 1.0f);
			//BL
			MyLODMeshData[aLOD].MyUV0[(thisX + 1) + (thisY * (rowLength))] = FVector2D((thisX + 1.0f) * 1.0f, thisY * 1.0f);
			//TL
			MyLODMeshData[aLOD].MyUV0[(thisX + 1) + ((thisY + 1) * (rowLength))] = FVector2D((thisX +1.0f)* 1.0f, (thisY+1.0f) * 1.0f);

		}
	}
}

// Main method for regenerating a zone
// Inplace update means the zone isn't moving it's just a LOD change (from 1 to 0)
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

	// Trash any existing instanced meshes
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
			PopulateMeshData(aLOD);
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
			&MyLODMeshData[aLOD].MyVertices,
			&MyLODMeshData[aLOD].MyTriangles,
			&MyLODMeshData[aLOD].MyNormals,
			&MyLODMeshData[aLOD].MyUV0,
			&MyLODMeshData[aLOD].MyVertexColors,
			&MyLODMeshData[aLOD].MyTangents,
			&MyLODMeshData[aLOD].MyHeightMap),
			*threadName,
			0, TPri_BelowNormal);
	}

	SetActorLocation(FVector(MyConfig.XUnits * MyConfig.UnitSize * MyOffset.x, MyConfig.YUnits * MyConfig.UnitSize * MyOffset.y, 0.0f));
}


// Performs the creation/update of the Procedural Mesh Component
void AZoneManager::UpdateMesh(const uint8 aLOD)
{
	// Create the mesh sections if they haven't been done already
	if (MyLODMeshStatus[aLOD] == eLODStatus::DRAWING_REQUIRES_CREATE)
	{
		// Only generate collsion if this is LOD0
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

// Spawns in instanced meshes for this block as defined in the biome configuration
bool AZoneManager::SpawnInstancedMeshesAtIndex(int32* aIndex)
{
	return true;
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

	// Run through each LOD we have on the zone
	for (auto& lod : MyLODMeshStatus)
	{
		// A thread has finished generating updated mesh data, and is ready to draw
		if (lod.Value == eLODStatus::READY_TO_DRAW)
		{
			lod.Value = eLODStatus::PENDING_DRAW;
			// Hand the thread token back to the world manager
			MyWorldManager->MyAvailableThreads.Enqueue(1);
			// Add a render task to the world manager
			MyWorldManager->MyRenderQueue.Enqueue(FZoneJob(MyZoneID, lod.Key, false));
			// Clean up
			delete Thread;
			Thread = NULL;
		}
		else if (lod.Value == eLODStatus::READY_TO_DRAW_REQUIRES_CREATE)
		{
			lod.Value = eLODStatus::PENDING_DRAW_REQUIRES_CREATE;
			// Hand the thread token back to the world manager
			MyWorldManager->MyAvailableThreads.Enqueue(1);
			// Add a render task to the world manager
			MyWorldManager->MyRenderQueue.Enqueue(FZoneJob(MyZoneID, lod.Key, false));
			// Clean up
			delete Thread;
			Thread = NULL;
		}


	}

	// If this is LOD 0 and the mesh has already been updated, start spawning biome instanced meshes
	if (MyLODMeshStatus.Contains(0) && MyLODMeshStatus[0] == eLODStatus::IDLE)
	{
		for (int i = 0; i < 50; ++i)
		{
			while (MyBlocksToSpawnFoliageOn > 0 && !SpawnInstancedMeshesAtIndex(&MyBlocksToSpawnFoliageOn));
		}
	}


}

// Raycasts vertically down from the given point and returns the hit location and normal to the caller 
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

// Return the location of the center of the zone
FVector AZoneManager::GetCentrePos()
{
	return  FVector(MyOffset.x * MyConfig.XUnits * MyConfig.UnitSize, MyOffset.y * MyConfig.YUnits * MyConfig.UnitSize, 0.0f);
}