// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "WorldGenerator.h"
#include "ZoneManager.generated.h"



UCLASS()
class CASHGENUE_API AZoneManager : public AActor
{
	GENERATED_BODY()
	UProceduralMeshComponent* MyProcMesh;

	WorldGenerator* worldGen;
	TArray<GridRow>* worldGrid;
	float gridSize;
	
	void CreateSection();
	void AddQuad(ZoneBlock* block, int32 aX, int32 aY);

	TArray<FVector> MyVertices;
	TArray<int32> MyTriangles;
	TArray<FVector> MyNormals;
	TArray<FVector2D> MyUV0;
	TArray<FColor> MyVertexColors;
	TArray<FProcMeshTangent> MyTangents;

	UFUNCTION(BlueprintCallable, Category = "Zone Manager")
	void SetupZone(int32 aX, int32 aY, float aUnitSize);
public:	
	// Sets default values for this actor's properties
	AZoneManager();
	~AZoneManager();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	void LoadTerrainGridAndGenerateMesh();
	
};
