
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
			}

		}
	}
}

/************************************************************************/
/*  Initial setup of the tile, creates components and material instance
/************************************************************************/
void ACGTile::SetupTile(CGPoint aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset)
{
	Offset.X = aOffset.X;
	Offset.Y = aOffset.Y;

	WorldOffset = aWorldOffset;
	TerrainConfigMaster = aTerrainConfig;

	// Disable tick if we're not doing lod transitions

	SetActorTickEnabled(TerrainConfigMaster->DitheringLODTransitions && aTerrainConfig->LODs.Num() > 1);

	for (int32 i = 0; i < aTerrainConfig->LODs.Num(); ++i)
	{

		FString compName = "RMC" + FString::FromInt(i);
		MeshComponents.Add(i, NewObject<URuntimeMeshComponent>(this, URuntimeMeshComponent::StaticClass(),*compName));

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
				MeshComponents[i]->CreateMeshSection(0, *aVertices, *aTriangles, *aNormals, *aUV0, *aVertexColors, *aTangents, TerrainConfigMaster->LODs[aLOD].isCollisionEnabled , EUpdateFrequency::Infrequent, TerrainConfigMaster->LODs[aLOD].isTesselationEnabled ? ESectionUpdateFlags::CalculateTessellationIndices | ESectionUpdateFlags::MoveArrays : ESectionUpdateFlags::MoveArrays);
				LODStatus.Add(i, ELODStatus::TRANSITION);
			}
			else {
				TArray<FVector> dummyUV1;
				MeshComponents[i]->UpdateMeshSection(0, *aVertices, *aTriangles, *aNormals, *aUV0, *aVertexColors, *aTangents, TerrainConfigMaster->LODs[aLOD].isTesselationEnabled ? ESectionUpdateFlags::CalculateTessellationIndices | ESectionUpdateFlags::MoveArrays : ESectionUpdateFlags::MoveArrays);
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

UMaterialInstanceDynamic* ACGTile::GetMaterialInstanceDynamic(const uint8 aLOD)
{
	if (aLOD < MaterialInstances.Num() - 1)
	{
		return MaterialInstances[aLOD];
	}

	return nullptr;
	
}
