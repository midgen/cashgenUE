#include "cashgen.h"
#include "ZoneManager.h"
#include "WorldManager.h"
#include "BiomeWeights.h"
#include "MeshData.h"
#include "FZoneGeneratorWorker.h"

AZoneManager::AZoneManager()
{
	PrimaryActorTick.bCanEverTick = true;
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	// A runtime mesh component for each of the LOD levels - will make this configurable
	MyRuntimeMeshComponents.Add(0, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("GeneratedMesh0")));
	MyRuntimeMeshComponents.Add(1, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("GeneratedMesh1")));
	MyRuntimeMeshComponents.Add(2, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("GeneratedMesh2")));

	this->SetActorEnableCollision(true);
	MyRuntimeMeshComponents[0]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MyRuntimeMeshComponents[1]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MyRuntimeMeshComponents[2]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MyRuntimeMeshComponents[0]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyRuntimeMeshComponents[1]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyRuntimeMeshComponents[2]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MyRuntimeMeshComponents[0]->bShouldSerializeMeshData = false;
	MyRuntimeMeshComponents[1]->bShouldSerializeMeshData = false;
	MyRuntimeMeshComponents[2]->bShouldSerializeMeshData = false;

	// LOD 10 = do not render
	currentlyDisplayedLOD = 10;
	previousDisplayedLOD = 10;
}

// Initial setup of the zone
void AZoneManager::SetupZone(const int32 aZoneID, AWorldManager* aWorldManager, const CGPoint aOffset, const FZoneConfig aZoneConfig, FVector* aWorldOffset)
{
	// The world grid offset of this zone
	MyOffset.x	= aOffset.x;
	MyOffset.y	= aOffset.y;

	// The full world offset (always apply this)
	worldOffset = aWorldOffset;
	// Config, manager pointers etc.
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
			MyInstancedMeshComponents.Add(NewObject<UHierarchicalInstancedStaticMeshComponent>(this, compName));
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

	MyLODMeshData[aLOD].MyVertices.Reserve(numXVerts * numYVerts);
	MyLODMeshData[aLOD].MyNormals.Reserve(numXVerts * numYVerts);
	MyLODMeshData[aLOD].MyUV0.Reserve(numXVerts * numYVerts);
	MyLODMeshData[aLOD].MyVertexColors.Reserve(numXVerts * numYVerts);
	MyLODMeshData[aLOD].MyTangents.Reserve(numXVerts * numYVerts);

	// Generate the per vertex data sets
	for (int32 i = 0; i < (numXVerts * numYVerts); ++i)
	{
		MyLODMeshData[aLOD].MyVertices.Emplace(0.0f);
		MyLODMeshData[aLOD].MyNormals.Emplace(0.0f, 0.0f, 1.0f);
		MyLODMeshData[aLOD].MyUV0.Emplace(0.0f, 0.0f);
		MyLODMeshData[aLOD].MyVertexColors.Emplace(FColor::Black);
		MyLODMeshData[aLOD].MyTangents.Emplace(0.0f, 0.0f, 0.0f);
	}

	// Heightmap needs to be larger than the mesh
	// Using vectors here is a bit wasteful, but it does make normal/tangent or any other
	// Geometric calculations based on the heightmap a bit easier. Easy enough to change to floats
	MyLODMeshData[aLOD].MyHeightMap.Reserve((numXVerts + 2) * (numYVerts + 2));
	for (int32 i = 0; i < (numXVerts + 2) * (numYVerts + 2); ++i)
	{
		MyLODMeshData[aLOD].MyHeightMap.Emplace(0.0f);
	}

	// Triangle indexes
	MyLODMeshData[aLOD].MyTriangles.Reserve((numXVerts - 1) * (numYVerts - 1) * 6);
	for (int32 i = 0; i < (numXVerts - 1) * (numYVerts - 1) * 6; ++i)
	{
		MyLODMeshData[aLOD].MyTriangles.Add(i);
	}

	CalculateTriangles(aLOD);

	// Fill the Biome Map, if it's LOD0
	MyLODMeshData[aLOD].BiomeWeightMap.Reserve(MyConfig.XUnits * MyConfig.YUnits);
	if (aLOD == 0)
	{
		for (int32 i = 0; i < MyConfig.XUnits * MyConfig.YUnits; ++i)
		{
			MyLODMeshData[aLOD].BiomeWeightMap.Emplace();
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
	float maxUV = aLOD == 0 ? 1.0f : 1.0f / aLOD;
	
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
			MyLODMeshData[aLOD].MyUV0[thisX + ((thisY + 1) * (rowLength))] = FVector2D(thisX * maxUV, (thisY+1.0f) * maxUV);
			//BR
			MyLODMeshData[aLOD].MyUV0[thisX + (thisY * (rowLength))] = FVector2D(thisX * maxUV, thisY * maxUV);
			//BL
			MyLODMeshData[aLOD].MyUV0[(thisX + 1) + (thisY * (rowLength))] = FVector2D((thisX + 1.0f) * maxUV, thisY * maxUV);
			//TL
			MyLODMeshData[aLOD].MyUV0[(thisX + 1) + ((thisY + 1) * (rowLength))] = FVector2D((thisX +1.0f)* maxUV, (thisY+1.0f) * maxUV);

		}
	}
}

// Main method for regenerating a zone
// Inplace update means the zone isn't moving it's just a LOD change (from 1 to 0)
void AZoneManager::RegenerateZone(const uint8 aLOD, const bool isInPlaceLODUpdate)
{
	// If this tile has been moved, hide the mesh
	if (!isInPlaceLODUpdate && previousDisplayedLOD != 10)
	{
		MyRuntimeMeshComponents[previousDisplayedLOD]->SetMeshSectionVisible(0, false);
	}

	LODTransitionOpacity = 1.0f;

	currentlyDisplayedLOD = aLOD;

	if (aLOD != 10)
	{
		FString threadName = "ZoneWorker" + FString::FromInt(MyOffset.x) + FString::FromInt(MyOffset.y) + FString::FromInt(aLOD);

		// If we haven't used this LOD before, populate the data structures and apply the material
		if (!MyLODMeshData.Contains(aLOD))
		{
			MyLODMeshStatus.Add(aLOD, eLODStatus::BUILDING_REQUIRES_CREATE);
			PopulateMeshData(aLOD);
			MyMaterialInstances.Add(aLOD, UMaterialInstanceDynamic::Create(MyConfig.TerrainMaterialInstanceParent, this));
			// Apply the debug LOD colors if enabled
			if (MyConfig.IsLODDebugEnabled) {
				switch(aLOD)
				{
					case 1:
						MyMaterialInstances[aLOD]->SetVectorParameterValue(FName("GrassColor"), FLinearColor::Red);
						break;
					case 2:
						MyMaterialInstances[aLOD]->SetVectorParameterValue(FName("GrassColor"), FLinearColor::Blue);
						break;
				}
			}
			// Otherwise use the specified grass color
			else {
				MyMaterialInstances[aLOD]->SetVectorParameterValue(FName("GrassColor"), MyConfig.GrassColor);
			}
			// Pass the other material parameters
			MyMaterialInstances[aLOD]->SetScalarParameterValue(FName("SlopeStart"), MyConfig.SlopeStart);
			MyMaterialInstances[aLOD]->SetScalarParameterValue(FName("SlopeEnd"), MyConfig.SlopeEnd);
			MyMaterialInstances[aLOD]->SetScalarParameterValue(FName("ShoreHeight"), MyConfig.ShoreHeight);
			MyMaterialInstances[aLOD]->SetVectorParameterValue(FName("SlopeColor"), MyConfig.SlopeColor);
			MyMaterialInstances[aLOD]->SetVectorParameterValue(FName("ShoreColor"), MyConfig.ShoreColor);
			MyRuntimeMeshComponents[aLOD]->SetMaterial(0, MyMaterialInstances[aLOD]);
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
			&MyLODMeshData[aLOD].MyHeightMap,
			&MyLODMeshData[aLOD].BiomeWeightMap),
			*threadName,
			0, TPri_BelowNormal);
	}

	SetActorLocation(FVector((MyConfig.XUnits * MyConfig.UnitSize * MyOffset.x) - worldOffset->X, (MyConfig.YUnits * MyConfig.UnitSize * MyOffset.y) - worldOffset->Y, 0.0f));
}


// Performs the creation/update of the Procedural Mesh Component
void AZoneManager::UpdateMesh(const uint8 aLOD)
{
	// Create the mesh sections if they haven't been done already
	if (MyLODMeshStatus[aLOD] == eLODStatus::DRAWING_REQUIRES_CREATE)
	{
		// Only generate collision if this is LOD0
		MyRuntimeMeshComponents[aLOD]->CreateMeshSection(0, MyLODMeshData[aLOD].MyVertices, MyLODMeshData[aLOD].MyTriangles, MyLODMeshData[aLOD].MyNormals, MyLODMeshData[aLOD].MyUV0, MyLODMeshData[aLOD].MyVertexColors, MyLODMeshData[aLOD].MyTangents, aLOD == 0, EUpdateFrequency::Infrequent);
		MyRuntimeMeshComponents[currentlyDisplayedLOD]->SetMeshSectionVisible(0, true);
		MyLODMeshStatus[aLOD] = SEGUE;
	}
	// Or just update them
	else if (MyLODMeshStatus[aLOD] == eLODStatus::DRAWING)
	{
		MyRuntimeMeshComponents[currentlyDisplayedLOD]->SetMeshSectionVisible(0, true);
		MyRuntimeMeshComponents[aLOD]->UpdateMeshSection(0, MyLODMeshData[aLOD].MyVertices, MyLODMeshData[aLOD].MyNormals, MyLODMeshData[aLOD].MyUV0, MyLODMeshData[aLOD].MyVertexColors, MyLODMeshData[aLOD].MyTangents);		
		MyLODMeshStatus[aLOD] = SEGUE;

		if (aLOD == 0)
		{
			for (auto& InstancedStaticMeshComponent : MyInstancedMeshComponents)
			{
				InstancedStaticMeshComponent->ClearInstances();
			}
			
		}
	}
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
		else if (lod.Value == eLODStatus::SEGUE)
		{
			if (LODTransitionOpacity >= -1.0f)
			{
				LODTransitionOpacity -= 0.02f;
				if (LODTransitionOpacity > 0.0f)
				{
					MyMaterialInstances[lod.Key]->SetScalarParameterValue(FName("TerrainOpacity"), 1.0f - LODTransitionOpacity);
				}
				else if (previousDisplayedLOD != 10 && previousDisplayedLOD != currentlyDisplayedLOD) 
				{
					MyMaterialInstances[previousDisplayedLOD]->SetScalarParameterValue(FName("TerrainOpacity"), LODTransitionOpacity + 1.0f);
				}
			}
			else
			{
				if (lod.Key == 0) { MyBlocksToSpawnFoliageOn = MyConfig.XUnits * MyConfig.YUnits; }
				if (previousDisplayedLOD != 10 && previousDisplayedLOD != currentlyDisplayedLOD) {
					MyRuntimeMeshComponents[previousDisplayedLOD]->SetMeshSectionVisible(0, false);
				}
				previousDisplayedLOD = currentlyDisplayedLOD;
				LODTransitionOpacity = 1.0f;
				lod.Value = eLODStatus::IDLE;	
			}
		}
		else if (currentlyDisplayedLOD == 0 && lod.Value == eLODStatus::IDLE)
		{
			for (int i = 0; i < 1; ++i)
			{
				while (MyBlocksToSpawnFoliageOn > 0 && !SpawnTreesAtIndex(&MyBlocksToSpawnFoliageOn));
			}
		}
	}
}

// Return the location of the center of the zone
FVector AZoneManager::GetCentrePos()
{
	return  FVector((MyOffset.x * MyConfig.XUnits * MyConfig.UnitSize) - worldOffset->X, (MyOffset.y * MyConfig.YUnits * MyConfig.UnitSize) - worldOffset->Y, 0.0f);
}

AZoneManager::~AZoneManager()
{
	// GC will do this anyway, but won't hurt to clear them down
	MyLODMeshData.Empty();
	MyLODMeshStatus.Empty();
}

bool AZoneManager::SpawnTreesAtIndex(int32* aIndex)
{
	int32 counter_Temp = (MyConfig.XUnits * MyConfig.YUnits) - (*aIndex);

	bool hasSpawnedOnThisBlock = false;

	int32 blockX, blockY;
	blockX = (counter_Temp % (MyConfig.YUnits));
	blockY = (counter_Temp / (MyConfig.YUnits));

	(*aIndex)--;

	int32 compIndex = 0;

	for (int32 BiomeIndex = 0; BiomeIndex < MyConfig.BiomeConfig.Num(); ++BiomeIndex)
	{
		for (int32 MeshIndex = 0; MeshIndex < MyConfig.BiomeConfig[BiomeIndex].MeshConfigs.Num(); ++MeshIndex)
		{
			for (int32 densityMultiplier = 0; densityMultiplier < MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].DensityMultiplier; ++densityMultiplier)
			{
				if (MyConfig.BiomeConfig[BiomeIndex].MeshConfigs[MeshIndex].DensityMultiplier * MyLODMeshData[currentlyDisplayedLOD].BiomeWeightMap[blockX + (blockY * MyConfig.YUnits)].BiomeWeights[BiomeIndex].Weight < FMath::FRandRange(0.0, 1.0))
				{
					hasSpawnedOnThisBlock = true;
					break;
				}

				FVector startPos = FVector(((MyOffset.x) * MyConfig.XUnits * MyConfig.UnitSize) - worldOffset->X + ((blockX)* MyConfig.UnitSize) + FMath::FRandRange(0.0f, MyConfig.UnitSize),
					(MyOffset.y * MyConfig.YUnits * MyConfig.UnitSize) - worldOffset->Y + ((blockY)* MyConfig.UnitSize) + FMath::FRandRange(0.0f, MyConfig.UnitSize), 50000.0f);

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
			++compIndex;
		}
	}

	return hasSpawnedOnThisBlock;
}

// Raycasts vertically down from the given point and returns the point it strikes the terrain
bool AZoneManager::GetGodCastHitPos(const FVector aVectorToStart, FVector* aHitPos, FVector* aNormalVector)
{
	const FName TraceTag("MyTraceTag");
	FCollisionQueryParams MyTraceParams = FCollisionQueryParams(FName(TEXT("TreeTrace")), true);
	MyTraceParams.bTraceComplex = false;
	MyTraceParams.bTraceAsyncScene = true;
	MyTraceParams.bReturnPhysicalMaterial = false;

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