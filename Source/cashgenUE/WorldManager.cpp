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

	for (auto& Elem : ZonesMaster)
	{
		Elem->isStale = false;
	}

	for (auto& Elem : ZoneOffsets)
	{
		Elem.Value.x -= delta.X;
		Elem.Value.y -= delta.Y;
	}

	if (delta.X < -0.1)
	{ 
		ZonesMaster[CurrentZones[ZonePos::R]]->isStale = true;
		ZonesMaster[CurrentZones[ZonePos::UR]]->isStale = true;
		ZonesMaster[CurrentZones[ZonePos::DR]]->isStale = true;

		NewZones[ZonePos::C] = CurrentZones[ZonePos::L];
		NewZones[ZonePos::U] = CurrentZones[ZonePos::UL];
		NewZones[ZonePos::D] = CurrentZones[ZonePos::DL];
		NewZones[ZonePos::L] = CurrentZones[ZonePos::R];
		NewZones[ZonePos::R] = CurrentZones[ZonePos::C];
		NewZones[ZonePos::UL] = CurrentZones[ZonePos::UR];
		NewZones[ZonePos::UR] = CurrentZones[ZonePos::U];
		NewZones[ZonePos::DL] = CurrentZones[ZonePos::DR];
		NewZones[ZonePos::DR] = CurrentZones[ZonePos::D];

		CurrentZones[ZonePos::C] = NewZones[ZonePos::C];
		CurrentZones[ZonePos::U] = NewZones[ZonePos::U];
		CurrentZones[ZonePos::D] = NewZones[ZonePos::D];
		CurrentZones[ZonePos::L] = NewZones[ZonePos::L];
		CurrentZones[ZonePos::R] = NewZones[ZonePos::R];
		CurrentZones[ZonePos::UL] = NewZones[ZonePos::UL];
		CurrentZones[ZonePos::UR] = NewZones[ZonePos::UR];
		CurrentZones[ZonePos::DL] = NewZones[ZonePos::DL];
		CurrentZones[ZonePos::DR] = NewZones[ZonePos::DR];

	}

	for (auto& Elem : CurrentZones)
	{
		if (ZonesMaster[Elem.Value]->isStale)
		{		
			ZonesMaster[Elem.Value]->RegenerateZone(ZoneOffsets[Elem.Key]);
		}
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

	NewZones.Add(ZonePos::C, 0);
	NewZones.Add(ZonePos::U, 0);
	NewZones.Add(ZonePos::D, 0);
	NewZones.Add(ZonePos::L, 0);
	NewZones.Add(ZonePos::R, 0);
	NewZones.Add(ZonePos::UL, 0);
	NewZones.Add(ZonePos::UR, 0);
	NewZones.Add(ZonePos::DL, 0);
	NewZones.Add(ZonePos::DR, 0);

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

