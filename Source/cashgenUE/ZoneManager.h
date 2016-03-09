// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "cashgenUE.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Point.h"
#include "ZoneBlock.h"
#include "ZoneManager.generated.h"


class AWorldManager;
struct GridRow
{
	TArray<ZoneBlock> blocks;
};

enum ZonePos { C, U, D, L, R, UL, UR, DL, DR };
struct ZoneConfig
{
	int32 XUnits;
	int32 YUnits;
	float UnitSize;
	float FloorDepth;
	float FloorHeight;
	float Persistence;
	float Frequency;
	float Amplitude;
	int32 Octaves;
	int32 RandomSeed;
};

UCLASS()
class CASHGENUE_API AZoneManager : public AActor
{
	GENERATED_BODY()
	UProceduralMeshComponent* MyProcMesh;
	UMaterial* MyMaterial;
	UMaterial* MyWaterMaterial;

	FRunnableThread* Thread;

	ZoneConfig MyConfig;
	
	AWorldManager* MyWorldManager;
	TArray<GridRow> MyZoneData;
	TArray<float> MyHeightMap;
	
	void CreateSection();
	void UpdateSection();
	void PopulateDataStructures();
	void InitialiseBlockPointers();
	void CalculateTriangles();
	void AddQuad(ZoneBlock* block, int32 aX, int32 aY);

	TArray<FVector> MyVertices;
	TArray<int32> MyTriangles;
	TArray<FVector> MyNormals;
	TArray<FVector2D> MyUV0;
	TArray<FColor> MyVertexColors;
	TArray<FProcMeshTangent> MyTangents;

	FVector CalcSurfaceNormalForTriangle(const int32 aStartTriangle);

	void CreateWaterPlane(float aWaterHeight);

public:	
	// Sets default values for this actor's properties
	AZoneManager();
	~AZoneManager();
	bool isStale = false;
	Point MyOffset;
	FVector MyTargetLocation;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	bool workerThreadCompleted = false;
	
	void SetupZone(AWorldManager* aWorldManager, Point aOffset, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, UMaterial* aWaterMaterial, float aFloorDepth, float aFloorHeight, float aWaterHeight, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed);
	void RegenerateZone();
};
