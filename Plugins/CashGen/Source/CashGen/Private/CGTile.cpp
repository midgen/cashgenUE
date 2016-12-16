
#include "cashgen.h"
#include "CGTile.h"

ACGTile::ACGTile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	CurrentLOD = 10;
	PreviousLOD = 10;
}

ACGTile::~ACGTile()
{

}

uint8 ACGTile::GetCurrentLOD()
{
	return CurrentLOD;
}

/************************************************************************/
/*  Move the tile and make it hidden pending a redraw
/************************************************************************/
void ACGTile::RepositionAndHide(uint8 aNewLOD)
{
	SetActorLocation(FVector((TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize * Offset.X) - WorldOffset.X, (TerrainConfigMaster->TileYUnits * TerrainConfigMaster->UnitSize * Offset.Y) - WorldOffset.Y, 0.0f));

	CurrentLOD = aNewLOD;

	for (auto& lod : LODStatus)
	{
		MeshComponents[lod.Key]->SetVisibility(false);
	}
}

void ACGTile::BeginPlay()
{
	Super::BeginPlay();
}

/************************************************************************/
/*  Tick just handles LOD transitions
/************************************************************************/
void ACGTile::Tick(float DeltaSeconds)
{
	for (auto& lod : LODStatus)
	{
		if (lod.Value == ELODStatus::TRANSITION)
		{
			if (LODTransitionOpacity >= -1.0f)
			{
				LODTransitionOpacity -= DeltaSeconds;

				if (LODTransitionOpacity > 0.0f)
				{
					MaterialInstances[lod.Key]->SetScalarParameterValue(FName("TerrainOpacity"), 1.0f - LODTransitionOpacity);
				}
				else if (PreviousLOD != 10 && PreviousLOD != CurrentLOD)
				{
					MaterialInstances[PreviousLOD]->SetScalarParameterValue(FName("TerrainOpacity"), LODTransitionOpacity + 1.0f);
				}
			}
			else
			{
				if (PreviousLOD != 10 && PreviousLOD != CurrentLOD) {
					MeshComponents[PreviousLOD]->SetVisibility(false);
				}
				
				LODTransitionOpacity = 1.0f;
				lod.Value = ELODStatus::CREATED;
				if (CurrentLOD == 0)
				{
					NumGrassInstancesToSpawn = 10000;
				}
			}

		}
	}

	if (NumGrassInstancesToSpawn > 0)
	{
		for (int i = 0; i < NumGrassInstancesPerFrame; ++i)
		{
			//FVector startPos = FVector(((Offset.X) * TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize) - WorldOffset->X) + FMath::FRandRange(0.0f, MyConfig.UnitSize),
			//	(MyOffset.y * MyConfig.YUnits * MyConfig.UnitSize) - worldOffset->Y + ((blockY)* MyConfig.UnitSize) + FMath::FRandRange(0.0f, MyConfig.UnitSize), 50000.0f);
			float tileWidth = TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize;
			FVector startPos = FVector(FMath::FRandRange(GetCentrePos().X - (tileWidth * 0.5f), GetCentrePos().X + (tileWidth * 0.5f)),
										FMath::FRandRange(GetCentrePos().Y - (tileWidth * 0.5f), GetCentrePos().Y + (tileWidth * 0.5f))
																, 50000.0f);

			FVector spawnPoint = FVector(0.0f, 0.0f, 0.0f);
			FVector normalVector = FVector(0.0f);
			if (GetGodCastHitPos(startPos, &spawnPoint, &normalVector))
			{
				FRotator rotation = FRotator(0.0f, FMath::FRandRange(-90.f, 90.0f), 0.0f);

				rotation += normalVector.Rotation() + FRotator(-90.0f, 0.0f, 0.0f);

				GrassHISM->AddInstance(FTransform(rotation, spawnPoint, FVector(1.0f)));
			}
			NumGrassInstancesToSpawn--;
		}

		
	}


}

// Raycasts vertically down from the given point and returns the point it strikes the terrain
bool ACGTile::GetGodCastHitPos(const FVector aVectorToStart, FVector* aHitPos, FVector* aNormalVector)
{
	const FName TraceTag("MyTraceTag");
	GetWorld()->DebugDrawTraceTag = TraceTag;
	FCollisionQueryParams MyTraceParams = FCollisionQueryParams(FName(TEXT("TreeTrace")), true);
	MyTraceParams.bTraceComplex = false;
	MyTraceParams.bTraceAsyncScene = true;
	MyTraceParams.bReturnPhysicalMaterial = false;

	FCollisionResponseParams MyResponseParams = FCollisionResponseParams();

	FHitResult MyHitResult(ForceInit);

	FVector MyCastDirection = FVector(0.0f, 0.0f, -1.0f);

	if (GetWorld()->LineTraceSingleByChannel(MyHitResult, aVectorToStart, aVectorToStart + (MyCastDirection * 900000.0f), ECC_Visibility, MyTraceParams, MyResponseParams))
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

/************************************************************************/
/*  Initial setup of the tile, creates components and material instance
/************************************************************************/
void ACGTile::SetupTile(CGPoint aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset, UHierarchicalInstancedStaticMeshComponent* aGrassComp)
{
	Offset.X = aOffset.X;
	Offset.Y = aOffset.Y;

	WorldOffset = aWorldOffset;
	TerrainConfigMaster = aTerrainConfig;

	GrassHISM = aGrassComp;

	for (int32 i = 0; i < aTerrainConfig->LODs.Num(); ++i)
	{

		FString compName = "RMC" + FString::FromInt(i);
		MeshComponents.Add(i, NewObject<URuntimeMeshComponent>(this, URuntimeMeshComponent::StaticClass(),*compName));

		MeshComponents[i]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		MeshComponents[i]->BodyInstance.SetResponseToAllChannels(ECR_Block);

		MeshComponents[i]->bShouldSerializeMeshData = false;

		MeshComponents[i]->RegisterComponent();

		LODStatus.Add(i, ELODStatus::NOT_CREATED);

		if (TerrainConfigMaster->TerrainMaterialInstanceParent != nullptr)
		{
			MaterialInstances.Add(i, UMaterialInstanceDynamic::Create(TerrainConfigMaster->TerrainMaterialInstanceParent, this));
			MeshComponents[i]->SetMaterial(0, MaterialInstances[i]);
		}
	}
}
 /************************************************************************/
 /*  Updates the mesh for a given LOD and starts the transition effects  
 /************************************************************************/
void ACGTile::UpdateMesh(uint8 aLOD, bool aIsInPlaceUpdate, TArray<FVector>*	aVertices,
	TArray<int32>*	aTriangles,
	TArray<FVector>*	aNormals,
	TArray<FVector2D>*	aUV0,
	TArray<FColor>*		aVertexColors,
	TArray<FRuntimeMeshTangent>* aTangents)
{
	PreviousLOD = CurrentLOD;
	CurrentLOD = aLOD;
	LODTransitionOpacity = 1.0f;

	for (int32 i = 0; i < TerrainConfigMaster->LODs.Num(); ++i)
	{
		if (i == aLOD) {
			if (LODStatus[i] == ELODStatus::NOT_CREATED) {
				MeshComponents[i]->CreateMeshSection(0, *aVertices, *aTriangles, *aNormals, *aUV0, *aVertexColors, *aTangents, TerrainConfigMaster->LODs[aLOD].isCollisionEnabled , EUpdateFrequency::Infrequent, TerrainConfigMaster->LODs[aLOD].isTesselationEnabled ? ESectionUpdateFlags::CalculateTessellationIndices : ESectionUpdateFlags::None);
				LODStatus.Add(i, ELODStatus::TRANSITION);
			}
			else {
				TArray<FVector> dummyUV1;
				MeshComponents[i]->UpdateMeshSection(0, *aVertices, *aTriangles, *aNormals, *aUV0, *aVertexColors, *aTangents, TerrainConfigMaster->LODs[aLOD].isTesselationEnabled ? ESectionUpdateFlags::CalculateTessellationIndices : ESectionUpdateFlags::None);
				LODStatus.Add(i, ELODStatus::TRANSITION);
			}

			MeshComponents[i]->SetVisibility(true);
		}
		else if (!aIsInPlaceUpdate)
		{
			MeshComponents[i]->SetVisibility(false);
		}
	}

}

/************************************************************************/
/*  Gets te position of the center of the tile, used for LODs
/************************************************************************/
FVector ACGTile::GetCentrePos()
{
	return  FVector(((Offset.X + 0.5f) * TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize) - WorldOffset.X, ((Offset.Y + 0.5f) * TerrainConfigMaster->TileYUnits * TerrainConfigMaster->UnitSize) - WorldOffset.Y, 0.0f);
}
