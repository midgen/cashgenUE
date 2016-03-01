// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "cashgenUE.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "WorldGenerator.h"
#include "Point.h"
#include "ZoneManager.generated.h"


class AWorldManager;
enum ZonePos { C, U, D, L, R, UL, UR, DL, DR };
struct ZoneConfig
{
	int32 XUnits;
	int32 YUnits;
	float UnitSize;
	float Floor;
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

	FRunnableThread* Thread;

	ZoneConfig MyConfig;
	Point MyOffset;

	WorldGenerator* worldGen;
	TArray<GridRow> MyZoneData;
	TArray<float> MyHeightMap;
	
	void CreateSection();
	void UpdateSection();
	void AddQuad(ZoneBlock* block, int32 aX, int32 aY);

	TArray<FVector> MyVertices;
	TArray<int32> MyTriangles;
	TArray<FVector> MyNormals;
	TArray<FVector2D> MyUV0;
	TArray<FColor> MyVertexColors;
	TArray<FProcMeshTangent> MyTangents;

	FVector CalcSurfaceNormalForTriangle(const int32 aStartTriangle);

public:	
	// Sets default values for this actor's properties
	AZoneManager();
	~AZoneManager();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	void LoadTerrainGridAndGenerateMesh(bool isNew);

	bool workerThreadCompleted = false;
	
	void SetupZone(Point aOffset, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, float aFloor, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed);
	void RegenerateZone(Point aOffset);
};
