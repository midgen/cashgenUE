// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "ZoneManager.h"


// Sets default values
AZoneManager::AZoneManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;

	MyProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));

	CreateSection();
}

// Called when the game starts or when spawned
void AZoneManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AZoneManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AZoneManager::CreateSection()
{

	MyVertices.Add(FVector(0, 0, 0));
	MyVertices.Add(FVector(0, 100, 0));
	MyVertices.Add(FVector(0, 0, 100));

	MyTriangles.Add(0);
	MyTriangles.Add(1);
	MyTriangles.Add(2);

	MyNormals.Add(FVector(1, 0, 0));
	MyNormals.Add(FVector(1, 0, 0));
	MyNormals.Add(FVector(1, 0, 0));

	MyUV0.Add(FVector2D(0, 0));
	MyUV0.Add(FVector2D(0, 10));
	MyUV0.Add(FVector2D(10, 10));

	MyVertexColors.Add(FColor(100, 100, 100, 100));
	MyVertexColors.Add(FColor(100, 100, 100, 100));
	MyVertexColors.Add(FColor(100, 100, 100, 100));

	MyTangents.Add(FProcMeshTangent(1, 1, 1));
	MyTangents.Add(FProcMeshTangent(1, 1, 1));
	MyTangents.Add(FProcMeshTangent(1, 1, 1));

	MyProcMesh->CreateMeshSection(1, MyVertices, MyTriangles, MyNormals, MyUV0, MyVertexColors, MyTangents, false);
	MyProcMesh->AttachTo(RootComponent);
}

