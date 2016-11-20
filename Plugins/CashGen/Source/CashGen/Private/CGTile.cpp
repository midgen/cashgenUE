
#include "cashgen.h"
#include "CGTile.h"

ACGTile::ACGTile()
{
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	MeshComponents.Add(0, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RMC0")));
	MeshComponents.Add(1, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RMC1")));
	MeshComponents.Add(2, CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RMC2")));
	this->SetActorEnableCollision(true);

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

}

void ACGTile::Tick(float DeltaSeconds)
{

}

void ACGTile::SetupTile(CGPoint aOffset, FCGTerrainConfig* aTerrainConfig, FVector aWorldOffset)
{
	Offset.X = aOffset.X;
	Offset.Y = aOffset.Y;
	WorldOffset = aWorldOffset;

	TerrainConfigMaster = aTerrainConfig;

}

void ACGTile::UpdateMesh(uint8 aLOD, FCGMeshData* aMeshData)
{
	for (int32 i = 0; i < 3; ++i)
	{
		if (i == aLOD) {
			if (LODStatus[i] == ELODStatus::NOT_CREATED) {
				MeshComponents[i]->CreateMeshSection(0, aMeshData->Vertices, aMeshData->Triangles, aMeshData->Normals, aMeshData->UV0, aMeshData->VertexColors, aMeshData->Tangents, aLOD == 0, EUpdateFrequency::Infrequent );
				LODStatus.Add(i, ELODStatus::CREATED);
			}
			else {
				MeshComponents[i]->UpdateMeshSection(0, aMeshData->Vertices, aMeshData->Triangles, aMeshData->Normals, aMeshData->UV0, aMeshData->VertexColors, aMeshData->Tangents);
			}

			MeshComponents[i]->SetMeshSectionVisible(0,true);
		}
		else {
			MeshComponents[i]->SetMeshSectionVisible(0, false);
		}
	}
}

FVector ACGTile::GetCentrePos()
{
	FVector result = FVector();

	return result;
}