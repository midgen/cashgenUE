// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "CashgenBaseActor.h"


// Sets default values
ACashgenBaseActor::ACashgenBaseActor()
{
	PrimaryActorTick.bCanEverTick = false;

	procMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	CreateSection();
}

// Called when the game starts or when spawned
void ACashgenBaseActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACashgenBaseActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ACashgenBaseActor::CreateSection()
{
	vertices.Add(FVector(0, 0, 0));
	vertices.Add(FVector(0, 100, 0));
	vertices.Add(FVector(0, 0, 100));

	triangles.Add(0);
	triangles.Add(1);
	triangles.Add(2);

	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));
	normals.Add(FVector(1, 0, 0));

	uv0.Add(FVector2D(0, 0));
	uv0.Add(FVector2D(0, 10));
	uv0.Add(FVector2D(10, 10));

	vertexColours.Add(FColor(100, 100, 100, 100));
	vertexColours.Add(FColor(100, 100, 100, 100));
	vertexColours.Add(FColor(100, 100, 100, 100));

	procMesh->CreateMeshSection(1, vertices, triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
}