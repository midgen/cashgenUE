// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "WorldManager.h"


// Sets default values
AWorldManager::AWorldManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ZoneOffsets.Add(ZonePos::C, Point(0, 0));
	ZoneOffsets.Add(ZonePos::U, Point(0, 1));
	ZoneOffsets.Add(ZonePos::D, Point(0, -1));
	ZoneOffsets.Add(ZonePos::L, Point(1, 0));
	ZoneOffsets.Add(ZonePos::R, Point(-1, 0));
	ZoneOffsets.Add(ZonePos::UL, Point(1, 1));
	ZoneOffsets.Add(ZonePos::UR, Point(-1, 1));
	ZoneOffsets.Add(ZonePos::DL, Point(1, -1));
	ZoneOffsets.Add(ZonePos::DR, Point(-1, -1));

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
		HandleZoneChange(oldPos - newPos);
	}

	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, currentPlayerZone.ToString());
}

void AWorldManager::HandleZoneChange(FVector2D delta)
{
	GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Green, delta.ToString());

	for (auto& Elem : ZoneOffsets)
	{
		Elem.Value.x -= delta.X;
		Elem.Value.y -= delta.Y;
		
	}

	for (auto& Elem : CurrentZones)
	{
		ZonesMaster[Elem.Value]->SetActorLocation(FVector(MyXUnits * MyGridSize * ZoneOffsets[Elem.Key].x, MyYUnits * MyGridSize * ZoneOffsets[Elem.Key].y, 0.0f));
		ZonesMaster[Elem.Value]->RegenerateZone(ZoneOffsets[Elem.Key]);
		
	}
}

void AWorldManager::SpawnZones(AActor* aPlayerPawn, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, float aFloor, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed)
{
	world = GetWorld();
	MyXUnits = aX; MyYUnits = aY, MyGridSize = aUnitSize;
	MyFloor = aFloor; MyPersistence = aPersistence;
	MyFrequency = aFrequency; MyAmplitude = aAmplitude;
	MyOctaves = aOctaves; MySeed = aRandomseed;

	currentPlayerPawn = aPlayerPawn;
	// Setup the tracking maps

	CurrentZones.Add(ZonePos::C, 0);
	CurrentZones.Add(ZonePos::U, 1);
	CurrentZones.Add(ZonePos::D, 2);
	CurrentZones.Add(ZonePos::L, 3);
	CurrentZones.Add(ZonePos::R, 4);
	CurrentZones.Add(ZonePos::UL, 5);
	CurrentZones.Add(ZonePos::UR, 6);
	CurrentZones.Add(ZonePos::DL, 7);
	CurrentZones.Add(ZonePos::DR, 8);

	for (auto& Elem : CurrentZones)
	{
		ZonesMaster.Add(world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), FVector(MyXUnits * MyGridSize * ZoneOffsets[Elem.Key].x, MyYUnits * MyGridSize * ZoneOffsets[Elem.Key].y, 0.0f), FRotator(0.0f)));
	}

	// Now set them up!
	for (auto& Elem : CurrentZones)
	{
		ZonesMaster[Elem.Value]->SetupZone(ZoneOffsets[Elem.Key], aX, aY, aUnitSize, aMaterial, aFloor, aPersistence, aFrequency, aAmplitude, aOctaves, aRandomseed);
	}

}

