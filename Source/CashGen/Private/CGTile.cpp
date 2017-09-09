
#include "cashgen.h"
#include "CGTile.h"

DECLARE_CYCLE_STAT(TEXT("CashGen ~ RMCUpdate"), STAT_RMCUpdate, STATGROUP_CashGen);

ACGTile::ACGTile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	CurrentLOD = 10;
	PreviousLOD = 10;

	mySector = FIntVector2(0, 0);

	
}

ACGTile::~ACGTile()
{

}

bool ACGTile::TickTransition(float DeltaSeconds)
{
	for (auto& lod : LODStatus)
	{
		if (lod.Value == ELODStatus::TRANSITION && MaterialInstances.Num() > 0)
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
				return true;
			}

		}
	}
	return false;
}

/************************************************************************/
/*  Move the tile and make it hidden pending a redraw
/************************************************************************/
void ACGTile::RepositionAndHide(uint8 aNewLOD)
{
	SetActorLocation(FVector((TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize * mySector.X) - TerrainConfigMaster->TileOffset.X, (TerrainConfigMaster->TileYUnits * TerrainConfigMaster->UnitSize * mySector.Y) - TerrainConfigMaster->TileOffset.Y, 0.0f));

	SetActorHiddenInGame(true);

	CurrentLOD = aNewLOD;
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

}

/************************************************************************/
/*  Initial setup of the tile, creates components and material instance
/************************************************************************/
void ACGTile::UpdateSettings(FIntVector2 aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset)
{
	mySector.X = aOffset.X;
	mySector.Y = aOffset.Y;

	if (!IsInitalized)
	{
		WorldOffset = aWorldOffset;
		TerrainConfigMaster = aTerrainConfig;

		// Disable tick if we're not doing lod transitions

		SetActorTickEnabled(TerrainConfigMaster->DitheringLODTransitions && aTerrainConfig->LODs.Num() > 1);

		for (int32 i = 0; i < aTerrainConfig->LODs.Num(); ++i)
		{

			FString compName = "RMC" + FString::FromInt(i);
			MeshComponents.Add(i, NewObject<URuntimeMeshComponent>(this, URuntimeMeshComponent::StaticClass(), *compName));

			MeshComponents[i]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

			MeshComponents[i]->BodyInstance.SetResponseToAllChannels(ECR_Block);
			MeshComponents[i]->BodyInstance.SetResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
			MeshComponents[i]->bShouldSerializeMeshData = false;

			MeshComponents[i]->RegisterComponent();

			LODStatus.Add(i, ELODStatus::NOT_CREATED);

			// Use dynamic material instances and do LOD dithering
			if (TerrainConfigMaster->TerrainMaterial != nullptr && TerrainConfigMaster->DitheringLODTransitions && aTerrainConfig->LODs.Num() > 1)
			{
				MaterialInstances.Add(i, UMaterialInstanceDynamic::Create(TerrainConfigMaster->TerrainMaterial, this));
				MeshComponents[i]->SetMaterial(0, MaterialInstances[i]);
			}
			// Just use a static material
			else if (TerrainConfigMaster->TerrainMaterial)
			{
				Material = TerrainConfigMaster->TerrainMaterial;
				MeshComponents[i]->SetMaterial(0, Material);
			}
			// Or just a static material instance
			else if (TerrainConfigMaster->TerrainMaterialInstance && !TerrainConfigMaster->MakeDynamicMaterialInstance)
			{
				MaterialInstance = TerrainConfigMaster->TerrainMaterialInstance;
				MeshComponents[i]->SetMaterial(0, MaterialInstance);
			}
			else if (TerrainConfigMaster->TerrainMaterialInstance && TerrainConfigMaster->MakeDynamicMaterialInstance)
			{
				MaterialInstances.Add(i, UMaterialInstanceDynamic::Create(TerrainConfigMaster->TerrainMaterialInstance, this));
				MeshComponents[i]->SetMaterial(0, MaterialInstances[i]);
			}

		}

		IsInitalized = true;
	}

}
 /************************************************************************/
 /*  Updates the mesh for a given LOD and starts the transition effects  
 /************************************************************************/
void ACGTile::UpdateMesh(uint8 aLOD, bool aIsInPlaceUpdate, TArray<FRuntimeMeshVertexSimple>*	aVertices,
	TArray<int32>*	aTriangles)
{
	SCOPE_CYCLE_COUNTER(STAT_RMCUpdate);
	SetActorHiddenInGame(false);

	PreviousLOD = CurrentLOD;
	CurrentLOD = aLOD;
	LODTransitionOpacity = 1.0f;

	for (int32 i = 0; i < TerrainConfigMaster->LODs.Num(); ++i)
	{
		if (i == aLOD) {
			if (LODStatus[i] == ELODStatus::NOT_CREATED) {
				MeshComponents[i]->CreateMeshSection(0, *aVertices, *aTriangles, TerrainConfigMaster->LODs[aLOD].isCollisionEnabled , EUpdateFrequency::Infrequent, TerrainConfigMaster->LODs[aLOD].isTesselationEnabled ? ESectionUpdateFlags::CalculateTessellationIndices : ESectionUpdateFlags::None);
				LODStatus.Add(i, ELODStatus::TRANSITION);
			}
			else {
				MeshComponents[i]->UpdateMeshSection(0, *aVertices, *aTriangles, TerrainConfigMaster->LODs[aLOD].isTesselationEnabled ? ESectionUpdateFlags::CalculateTessellationIndices : ESectionUpdateFlags::None);
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

UMaterialInstanceDynamic* ACGTile::GetMaterialInstanceDynamic(const uint8 aLOD)
{
	if (aLOD < MaterialInstances.Num() - 1)
	{
		return MaterialInstances[aLOD];
	}

	return nullptr;
	
}
