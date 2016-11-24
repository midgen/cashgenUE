
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
				LODTransitionOpacity -= 0.02f;
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
					MeshComponents[PreviousLOD]->SetMeshSectionVisible(0, false);
				}
				PreviousLOD = CurrentLOD;
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

	for (auto& lod : LODStatus)
	{
		MaterialInstances.Add(lod.Key, UMaterialInstanceDynamic::Create(TerrainConfigMaster->TerrainMaterialInstanceParent, this));
		// Apply the debug LOD colors if enabled
		if (TerrainConfigMaster->IsLODDebugEnabled) {
			switch (lod.Key)
			{
			case 1:
				MaterialInstances[lod.Key]->SetVectorParameterValue(FName("GrassColor"), FLinearColor::Red);
				break;
			case 2:
				MaterialInstances[lod.Key]->SetVectorParameterValue(FName("GrassColor"), FLinearColor::Blue);
				break;
			}
		}
		// Otherwise use the specified grass color
		else {
			MaterialInstances[lod.Key]->SetVectorParameterValue(FName("GrassColor"), TerrainConfigMaster->GrassColor);
		}
		// Pass the other material parameters
		MaterialInstances[lod.Key]->SetScalarParameterValue(FName("SlopeStart"), TerrainConfigMaster->SlopeStart);
		MaterialInstances[lod.Key]->SetScalarParameterValue(FName("SlopeEnd"), TerrainConfigMaster->SlopeEnd);
		MaterialInstances[lod.Key]->SetScalarParameterValue(FName("ShoreHeight"), TerrainConfigMaster->ShoreHeight);
		MaterialInstances[lod.Key]->SetScalarParameterValue(FName("TreeLineHeight"), TerrainConfigMaster->TreeLine);
		MaterialInstances[lod.Key]->SetVectorParameterValue(FName("SlopeColor"), TerrainConfigMaster->SlopeColor);
		MaterialInstances[lod.Key]->SetVectorParameterValue(FName("ShoreColor"), TerrainConfigMaster->ShoreColor);
		MaterialInstances[lod.Key]->SetVectorParameterValue(FName("TreeLineColor"), TerrainConfigMaster->TreeLineColor);

		MeshComponents[lod.Key]->SetMaterial(0, MaterialInstances[lod.Key]);
	}
}

void ACGTile::UpdateMesh(uint8 aLOD, bool aIsInPlaceUpdate, TArray<FVector>*	aVertices,
	TArray<int32>*	aTriangles,
	TArray<FVector>*	aNormals,
	TArray<FVector2D>*	aUV0,
	TArray<FColor>*		aVertexColors,
	TArray<FRuntimeMeshTangent>* aTangents)
{

	if (!aIsInPlaceUpdate && PreviousLOD != 10)
	{
		MeshComponents[PreviousLOD]->SetMeshSectionVisible(0, false);
	}

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

			MeshComponents[i]->SetMeshSectionVisible(0,true);
		}
		else {
			MeshComponents[i]->SetMeshSectionVisible(0, false);
		}
	}

	SetActorLocation(FVector((TerrainConfigMaster->XUnits * TerrainConfigMaster->UnitSize * Offset.X) - WorldOffset.X, (TerrainConfigMaster->YUnits * TerrainConfigMaster->UnitSize * Offset.Y) - WorldOffset.Y, 0.0f));

}

FVector ACGTile::GetCentrePos()
{
	return  FVector(((Offset.X + 0.5f) * TerrainConfigMaster->XUnits * TerrainConfigMaster->UnitSize) - WorldOffset.X, ((Offset.Y + 0.5f) * TerrainConfigMaster->YUnits * TerrainConfigMaster->UnitSize) - WorldOffset.Y, 0.0f);
}
