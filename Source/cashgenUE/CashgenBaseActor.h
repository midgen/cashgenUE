// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "CashgenBaseActor.generated.h"


UCLASS()
class CASHGENUE_API ACashgenBaseActor : public AActor
{
	GENERATED_BODY()

		UProceduralMeshComponent* procMesh;

	void CreateSection();

	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uv0;
	TArray<FColor> vertexColours;


public:	
	// Sets default values for this actor's properties
	ACashgenBaseActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;



	
	
};
