// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ZoneManager.h"
#include "Point.h"
#include "WorldManager.generated.h"


UCLASS()
class CASHGENUE_API AWorldManager : public AActor
{
	GENERATED_BODY()

	TArray<AZoneManager*> ZonesMaster;
	TMap<ZonePos, int8> CurrentZones;

	TMap<ZonePos, Point> ZoneOffsets;

	TMap<ZonePos, int8> NewZones;
	AActor* currentPlayerPawn;
	UWorld* world;

	float unitSize;
	int32 xUnits;
	int32 yUnits;

	void HandleZoneChange(FVector2D delta);

public:	
	// Sets default values for this actor's properties
	AWorldManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = "World Manager")
	void SpawnZones(AActor* aPlayerPawn, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, float aFloor, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CashGen Manager")
		FVector2D currentPlayerZone;

};
