// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ZoneManager.generated.h"


UCLASS()
class CASHGENUE_API AZoneManager : public AActor
{
	GENERATED_BODY()
		UProceduralMeshComponent* MyProcMesh;
	
	void CreateSection();

	TArray<FVector> MyVertices;
	TArray<int32> MyTriangles;
	TArray<FVector> MyNormals;
	TArray<FVector2D> MyUV0;
	TArray<FColor> MyVertexColors;
	TArray<FProcMeshTangent> MyTangents;

public:	
	// Sets default values for this actor's properties
	AZoneManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
