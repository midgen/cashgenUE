
#include "cashgen.h"
#include "CGTile.h"

ACGTile::ACGTile()
{
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	MeshComponents.Add(0, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RMC0")));
	MeshComponents.Add(1, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RMC1")));
	MeshComponents.Add(2, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RMC2")));

	MeshComponents[0]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponents[1]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponents[2]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponents[0]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MeshComponents[1]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MeshComponents[2]->BodyInstance.SetResponseToAllChannels(ECR_Block);
	MeshComponents[0]->bShouldSerializeMeshData = false;
	MeshComponents[1]->bShouldSerializeMeshData = false;
	MeshComponents[2]->bShouldSerializeMeshData = false;

	LODStatus.Add(0, ELODStatus::NOT_CREATED);
	LODStatus.Add(1, ELODStatus::NOT_CREATED);
	LODStatus.Add(2, ELODStatus::NOT_CREATED);

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

void ACGTile::RepositionAndHide(uint8 aNewLOD)
{
	SetActorLocation(FVector((TerrainConfigMaster->XUnits * TerrainConfigMaster->UnitSize * Offset.X) - WorldOffset.X, (TerrainConfigMaster->YUnits * TerrainConfigMaster->UnitSize * Offset.Y) - WorldOffset.Y, 0.0f));

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
					//MaterialInstances[lod.Key]->SetScalarParameterValue(FName("TerrainOpacity"), 1.0f - LODTransitionOpacity);
				}
				else if (PreviousLOD != 10 && PreviousLOD != CurrentLOD)
				{
					//MaterialInstances[PreviousLOD]->SetScalarParameterValue(FName("TerrainOpacity"), LODTransitionOpacity + 1.0f);
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

void ACGTile::SetupTile(CGPoint aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset)
{
	Offset.X = aOffset.X;
	Offset.Y = aOffset.Y;

	WorldOffset = aWorldOffset;
	TerrainConfigMaster = aTerrainConfig;

	for (int32 i = 0; i < 3; ++i)
	{
		if (TerrainConfigMaster->TerrainMaterialInstanceParent != nullptr)
		{
			MaterialInstances.Add(i, UMaterialInstanceDynamic::Create(TerrainConfigMaster->TerrainMaterialInstanceParent, this));
			// Apply the debug LOD colors if enabled
			if (TerrainConfigMaster->IsLODDebugEnabled) {
				switch (i)
				{
				case 1:
					MaterialInstances[i]->SetVectorParameterValue(FName("GrassColor"), FLinearColor::Red);
					break;
				case 2:
					MaterialInstances[i]->SetVectorParameterValue(FName("GrassColor"), FLinearColor::Blue);
					break;
				}
			}
			// Otherwise use the specified grass color
			else {
				MaterialInstances[i]->SetVectorParameterValue(FName("GrassColorBase"), TerrainConfigMaster->GrassColorBase);
				MaterialInstances[i]->SetVectorParameterValue(FName("GrassColorSlope"), TerrainConfigMaster->GrassColorSlope);
			}
			// Pass the other material parameters
			MaterialInstances[i]->SetScalarParameterValue(FName("SlopeStart"), TerrainConfigMaster->SlopeStart);
			MaterialInstances[i]->SetScalarParameterValue(FName("SlopeEnd"), TerrainConfigMaster->SlopeEnd);
			MaterialInstances[i]->SetScalarParameterValue(FName("ShoreHeight"), TerrainConfigMaster->ShoreHeight);
			MaterialInstances[i]->SetScalarParameterValue(FName("TreeLineHeight"), TerrainConfigMaster->TreeLine);
			MaterialInstances[i]->SetVectorParameterValue(FName("ShoreColor"), TerrainConfigMaster->ShoreColor);
			MaterialInstances[i]->SetVectorParameterValue(FName("TreeLineColorBase"), TerrainConfigMaster->TreeLineColorBase);
			MaterialInstances[i]->SetVectorParameterValue(FName("TreeLineColorSlope"), TerrainConfigMaster->TreeLineColorSlope);

			MeshComponents[i]->SetMaterial(0, MaterialInstances[i]);
		}
		else if (TerrainConfigMaster->TerrainMatInstance != nullptr)
		{
			MeshComponents[i]->SetMaterial(0, TerrainConfigMaster->TerrainMatInstance);
		}
	}
}

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

	for (int32 i = 0; i < 3; ++i)
	{
		if (i == aLOD) {
			if (LODStatus[i] == ELODStatus::NOT_CREATED) {
				MeshComponents[i]->CreateMeshSection(0, *aVertices, *aTriangles, *aNormals, *aUV0, *aVertexColors, *aTangents, aLOD == 0, EUpdateFrequency::Infrequent );
				LODStatus.Add(i, ELODStatus::TRANSITION);
			}
			else {
				MeshComponents[i]->UpdateMeshSection(0, *aVertices, *aTriangles, *aNormals, *aUV0, *aVertexColors, *aTangents);
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

FVector ACGTile::GetCentrePos()
{
	return  FVector(((Offset.X + 0.5f) * TerrainConfigMaster->XUnits * TerrainConfigMaster->UnitSize) - WorldOffset.X, ((Offset.Y + 0.5f) * TerrainConfigMaster->YUnits * TerrainConfigMaster->UnitSize) - WorldOffset.Y, 0.0f);
}
