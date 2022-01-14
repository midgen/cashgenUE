#include "CGTile.h"
#include "Components/StaticMeshComponent.h"
#include "Struct/CGTerrainConfig.h"

#include <ProceduralMeshComponent/Public/ProceduralMeshComponent.h>

DECLARE_CYCLE_STAT(TEXT("CashGenStat ~ RMCUpdate"), STAT_RMCUpdate, STATGROUP_CashGenStat);

ACGTile::ACGTile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	CurrentLOD = 10;
	PreviousLOD = 10;

	mySector = FCGIntVector2(0, 0);
}

ACGTile::~ACGTile()
{
	if (myRegion)
	{
		delete myRegion;
		myRegion = nullptr;
	}
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
				if (PreviousLOD != 10 && PreviousLOD != CurrentLOD)
				{
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

/************************************************************************
 * Move the tile and make it hidden pending a redraw
 ************************************************************************/
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

void ACGTile::Tick(float DeltaSeconds)
{
}

/************************************************************************
 * Initial setup of the tile, creates components and material instance
 ************************************************************************/
void ACGTile::UpdateSettings(FCGIntVector2 aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset)
{
	mySector.X = aOffset.X;
	mySector.Y = aOffset.Y;

	if (!IsInitalized)
	{
		WorldOffset = aWorldOffset;
		TerrainConfigMaster = aTerrainConfig;

		// Disable tick if we're not doing lod transitions

		SetActorTickEnabled(TerrainConfigMaster->DitheringLODTransitions && aTerrainConfig->LODs.Num() > 1);

		FString waterCompName = "WaterSMC";
		FTransform waterTransform = FTransform(FRotator::ZeroRotator, FVector(TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize * 0.5f, TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize * 0.5f, 0.0f), FVector(TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize * 0.01f, TerrainConfigMaster->TileYUnits * TerrainConfigMaster->UnitSize * 0.01f, 1.0f));
		MyWaterMeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), *waterCompName);
		MyWaterMeshComponent->SetStaticMesh(TerrainConfigMaster->WaterMesh);
		MyWaterMeshComponent->SetRelativeTransform(waterTransform);
		MyWaterMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		MyWaterMeshComponent->RegisterComponent();

		myWaterMaterialInstance = UMaterialInstanceDynamic::Create(TerrainConfigMaster->WaterMaterialInstance, this);
		MyWaterMeshComponent->SetMaterial(0, myWaterMaterialInstance);

		for (int32 i = 0; i < aTerrainConfig->LODs.Num(); ++i)
		{

			FString compName = "RMC" + FString::FromInt(i);
			MeshComponents.Add(i, NewObject<UProceduralMeshComponent>(this, UProceduralMeshComponent::StaticClass(), *compName));
			MeshComponents[i]->SetRelativeTransform(FTransform());
			MeshComponents[i]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

			MeshComponents[i]->BodyInstance.SetResponseToAllChannels(ECR_Block);
			MeshComponents[i]->BodyInstance.SetResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

			MeshComponents[i]->bCastDynamicShadow = i == 0 ? TerrainConfigMaster->CastShadows : false;
			MeshComponents[i]->bCastStaticShadow = i == 0 ? TerrainConfigMaster->CastShadows : false;

			LODStatus.Add(i, ELODStatus::NOT_CREATED);

			// Create material instances
			if (TerrainConfigMaster->TerrainMaterialInstance && !TerrainConfigMaster->MakeDynamicMaterialInstance)
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

		if (TerrainConfigMaster->GenerateSplatMap)
		{
			myTexture = UTexture2D::CreateTransient(TerrainConfigMaster->TileXUnits, TerrainConfigMaster->TileYUnits, EPixelFormat::PF_B8G8R8A8);
			myTexture->AddressX = TA_Clamp;
			myTexture->AddressY = TA_Clamp;

			myTexture->UpdateResource();

			myRegion = new FUpdateTextureRegion2D();
			myRegion->Height = TerrainConfigMaster->TileYUnits;
			myRegion->Width = TerrainConfigMaster->TileXUnits;
			myRegion->SrcX = 0;
			myRegion->SrcY = 0;
			myRegion->DestX = 0;
			myRegion->DestY = 0;
		}

		IsInitalized = true;
	}
}

/************************************************************************
 *  Draw a simple quad to use as the water plane
 ************************************************************************/
bool ACGTile::CreateWaterMesh()
{

	if (MeshComponents.Num() > 0)
	{
		TArray<FVector> myPositions;
		TArray<FVector> myNormals;
		TArray<FProcMeshTangent> myTangents;
		TArray<FVector2D> myUV0;
		FVector normal;
		normal = FVector(0.0f, 0.0f, 1.0f);
		FProcMeshTangent tangent, tangentX;

		myPositions.Reserve(4);
		myNormals.Reserve(4);
		myUV0.Reserve(4);

		int32 i = 0;
		myPositions.Emplace();
		myNormals.Emplace();
		myTangents.Emplace();
		myUV0.Emplace();
		myPositions[i].X = 0.0f;
		myPositions[i].Y = 0.0f;
		myPositions[i].Z = 0.0f;
		myUV0[i] = FVector2D(0.0f, 0.0f);
		tangent = FProcMeshTangent(0.0f, 1.0f, 0.0f);
		myNormals[i] = normal;
		myTangents[i] = tangent;

		++i;

		myPositions.Emplace();
		myNormals.Emplace();
		myTangents.Emplace();
		myUV0.Emplace();
		myPositions[i].X = 0.0f;
		myPositions[i].Y = TerrainConfigMaster->TileYUnits * TerrainConfigMaster->UnitSize;
		myPositions[i].Z = 0.0f;
		myUV0[i] = FVector2D(1.0f, 0.0f);
		tangentX = FProcMeshTangent(0.0f, 1.0f, 0.0f);
		myNormals[i] = normal;
		myTangents[i] = tangentX;

		++i;

		myPositions.Emplace();
		myNormals.Emplace();
		myTangents.Emplace();
		myUV0.Emplace();
		myPositions[i].X = TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize;
		myPositions[i].Y = TerrainConfigMaster->TileYUnits * TerrainConfigMaster->UnitSize;
		myPositions[i].Z = 0.0f;
		myUV0[i] = FVector2D(1.0f, 1.0f);

		tangentX = FProcMeshTangent(0.0f, 1.0f, 0.0f);
		myNormals[i] = normal;
		myTangents[i] = tangentX;
		++i;

		myPositions.Emplace();
		myNormals.Emplace();
		myTangents.Emplace();
		myUV0.Emplace();
		myPositions[i].X = TerrainConfigMaster->TileXUnits * TerrainConfigMaster->UnitSize;
		myPositions[i].Y = 0.0f;
		myPositions[i].Z = 0.0f;
		myUV0[i] = FVector2D(0.0f, 1.0f);
		tangentX = FProcMeshTangent(0.0f, 1.0f, 0.0f);
		myNormals[i] = normal;
		myTangents[i] = tangentX;

		TArray<int32> myIndices;
		myIndices.Reserve(6);
		myIndices.Emplace(0);
		myIndices.Emplace(1);
		myIndices.Emplace(2);
		myIndices.Emplace(2);
		myIndices.Emplace(3);
		myIndices.Emplace(0);

		MeshComponents[0]->CreateMeshSection(1, myPositions, myIndices, myNormals, myUV0, TArray<FColor>(), myTangents, true);

		myWaterMaterialInstance = UMaterialInstanceDynamic::Create(TerrainConfigMaster->WaterMaterialInstance, this);
		MeshComponents[0]->SetMaterial(1, myWaterMaterialInstance);

		return true;
	}
	return false;
}

/************************************************************************
  *  Updates the mesh for a given LOD and starts the transition effects  
  ************************************************************************/
void ACGTile::UpdateMesh(uint8 aLOD, bool aIsInPlaceUpdate,
	TArray<FVector>& aPositions, TArray<FVector>& aNormals, TArray<FProcMeshTangent>& aTangents, TArray<FVector2D>& aUV0s, TArray<FColor>& aColours, TArray<int32>& aTriangles, TArray<FColor>& aTextureData)
{
	SCOPE_CYCLE_COUNTER(STAT_RMCUpdate);
	SetActorHiddenInGame(false);

	PreviousLOD = CurrentLOD;
	CurrentLOD = aLOD;
	LODTransitionOpacity = 1.0f;

	for (int32 i = 0; i < TerrainConfigMaster->LODs.Num(); ++i)
	{
		if (i == aLOD)
		{
			if (LODStatus[i] == ELODStatus::NOT_CREATED)
			{
				MeshComponents[i]->CreateMeshSection(0, aPositions, aTriangles, aNormals, aUV0s, aColours, aTangents, TerrainConfigMaster->LODs[aLOD].isCollisionEnabled);
				MeshComponents[i]->RegisterComponent();
				LODStatus.Add(i, ELODStatus::TRANSITION);
			}
			else
			{
				MeshComponents[i]->UpdateMeshSection(0, aPositions, aNormals, aUV0s, aColours, aTangents);
				LODStatus.Add(i, ELODStatus::TRANSITION);
			}

			MeshComponents[i]->SetVisibility(true);
		}
		else if (!aIsInPlaceUpdate)
		{
			MeshComponents[i]->SetVisibility(false);
		}
	}

	if (aLOD == 0 && TerrainConfigMaster->GenerateSplatMap && TerrainConfigMaster->MakeDynamicMaterialInstance && MaterialInstances.Num() > 0)
	{

		myTexture->UpdateTextureRegions(0, 1, myRegion, 4 * TerrainConfigMaster->TileXUnits, 4, (uint8*)aTextureData.GetData());

		MaterialInstances[0]->SetTextureParameterValue("SplatMap", myTexture);
		myWaterMaterialInstance->SetTextureParameterValue("SplatMap", myTexture);
	}

	if (TerrainConfigMaster->LODs[aLOD].isCollisionEnabled)
	{
		MyWaterMeshComponent->SetCollisionEnabled(TerrainConfigMaster->WaterCollision);
	}
	else
	{
		MyWaterMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
