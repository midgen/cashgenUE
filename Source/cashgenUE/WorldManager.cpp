// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "WorldManager.h"


// Sets default values
AWorldManager::AWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWorldManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWorldManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector2D oldPos = currentPlayerZone;
	
	if (currentPlayerPawn)
	{
		currentPlayerZone.X = floor(currentPlayerPawn->GetActorLocation().X / (MyGridSize* MyXUnits));
		currentPlayerZone.Y = floor(currentPlayerPawn->GetActorLocation().Y / (MyGridSize* MyYUnits));
	}

	FVector2D newPos = currentPlayerZone;

	if (oldPos.X != newPos.X || oldPos.Y != newPos.Y)
	{
		HandleZoneChange(newPos - oldPos);
	}

	for (int i = 0; i < UpdatesPerFrame; ++i)
	{
		int32 indexToRegen = -1;
		if (MyRegenQueue.Dequeue(indexToRegen))
		{
			if (indexToRegen >= 0)
			{
				ZonesMaster[indexToRegen]->RegenerateZone();
			}
		}
	}



	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, currentPlayerZone.ToString());
}

void AWorldManager::HandleZoneChange(FVector2D delta)
{
	GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Green, delta.ToString());

	//zone->MyOffset.x -= delta.X;
	//zone->MyOffset.y -= delta.Y;

	int32 minX = 0;
	int32 maxX = 0;
	int32 minY = 0;
	int32 maxY = 0;

	// Find our min/max TODO: optimise out and track
	for (int i = 0; i < ZonesMaster.Num(); ++i)
	{
		if (i == 0) {
			minX = ZonesMaster[i]->MyOffset.x;
			maxX = ZonesMaster[i]->MyOffset.x;
			minY = ZonesMaster[i]->MyOffset.y;
			maxY = ZonesMaster[i]->MyOffset.y;
		}
		if (ZonesMaster[i]->MyOffset.x < minX) {
			minX = ZonesMaster[i]->MyOffset.x;
		}
		if (ZonesMaster[i]->MyOffset.x > maxX) {
			maxX = ZonesMaster[i]->MyOffset.x;
		}
		if (ZonesMaster[i]->MyOffset.y < minY) {
			minY = ZonesMaster[i]->MyOffset.y;
		}
		if (ZonesMaster[i]->MyOffset.y > maxY) {
			maxY = ZonesMaster[i]->MyOffset.y;
		}
	}



	for (int i = 0; i < ZonesMaster.Num(); ++i)
	{
		ZonesMaster[i]->isStale = false;

		// Moving left on X axis, flip left column to the right
		if (delta.X < -0.1 && ZonesMaster[i]->MyOffset.x == maxX)
		{
			ZonesMaster[i]->MyOffset.x = minX - 1;
			MyRegenQueue.Enqueue(i);
			//ZonesMaster[i]->isStale = true;
		}
		// Moving right on X, flip right column to left
		if (delta.X > 0.1 && ZonesMaster[i]->MyOffset.x == minX)
		{
			ZonesMaster[i]->MyOffset.x = maxX + 1;
			MyRegenQueue.Enqueue(i);
			//ZonesMaster[i]->isStale = true;
		}
		// Movin down on Y, flip top row to bottom
		if (delta.Y < -0.1 && ZonesMaster[i]->MyOffset.y == maxY)
		{
			ZonesMaster[i]->MyOffset.y = minY - 1;
			MyRegenQueue.Enqueue(i);
			//ZonesMaster[i]->isStale = true;
		}
		// Moving up on Y, flip bottom wor to top
		if (delta.Y > 0.1 && ZonesMaster[i]->MyOffset.y == minY)
		{
			ZonesMaster[i]->MyOffset.y = maxY + 1;
			MyRegenQueue.Enqueue(i);
			//ZonesMaster[i]->isStale = true;
		}

	}
}



void AWorldManager::SpawnZones(AActor* aPlayerPawn, int32 aNumXZones, int32 aNumYZones, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, float aFloor, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed)
{
	world = GetWorld();
	MyNumXZones = aNumXZones;
	MyNumYZones = aNumYZones;
	MyXUnits = aX; MyYUnits = aY, MyGridSize = aUnitSize;
	MyFloor = aFloor; MyPersistence = aPersistence;
	MyFrequency = aFrequency; MyAmplitude = aAmplitude;
	MyOctaves = aOctaves; MySeed = aRandomseed;

	currentPlayerPawn = aPlayerPawn;

	currentPlayerPawn->SetActorLocation(FVector(MyNumXZones * MyXUnits * MyGridSize * 0.5f, MyNumYZones * MyYUnits * MyGridSize * 0.5f, 50.0f));

	for (int32 i = 0; i < MyNumXZones * MyNumYZones; ++i )
	{
		ZonesMaster.Add(world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), FVector(MyXUnits * MyGridSize * GetXYfromIdx(i).x, MyYUnits * MyGridSize * GetXYfromIdx(i).y, 0.0f), FRotator(0.0f)));
	}

	for (int i = 0; i < ZonesMaster.Num(); ++i)
	{
		ZonesMaster[i]->SetupZone(GetXYfromIdx(i), MyXUnits, MyYUnits, MyGridSize, aMaterial, MyFloor, MyPersistence, MyFrequency, MyAmplitude, MyOctaves, MySeed);
	}
}

