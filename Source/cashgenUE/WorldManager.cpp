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
	ZoneOffsets.Add(ZonePos::L, Point(-1, 0));
	ZoneOffsets.Add(ZonePos::R, Point(1, 0));
	ZoneOffsets.Add(ZonePos::UL, Point(-1, 1));
	ZoneOffsets.Add(ZonePos::UR, Point(1, 1));
	ZoneOffsets.Add(ZonePos::DL, Point(-1, -1));
	ZoneOffsets.Add(ZonePos::DR, Point(1, -1));

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
		currentPlayerZone.X = floor(currentPlayerPawn->GetActorLocation().X / (unitSize* xUnits));
		currentPlayerZone.Y = floor(currentPlayerPawn->GetActorLocation().Y / (unitSize* yUnits));
	}

	FVector2D newPos = currentPlayerZone;

	if (oldPos.X != newPos.X || oldPos.Y != newPos.Y)
	{
		HandleZoneChange(oldPos, newPos);
	}

	GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, currentPlayerZone.ToString());
}

void AWorldManager::HandleZoneChange(FVector2D aOldZone, FVector2D aNewZone)
{
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, TEXT("Zone Changed!"));
}

void AWorldManager::SpawnZones(AActor* aPlayerPawn, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, float aFloor, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed)
{
	world = GetWorld();
	xUnits = aX; yUnits = aY, unitSize = aUnitSize;
	currentPlayerPawn = aPlayerPawn;
	// Spawn in the 9 zones
	Zones.Add(ZonePos::C, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation(), FRotator(0.0f)));
	Zones.Add(ZonePos::U, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation() + FVector(0.0f, aY * aUnitSize, 0.0f), FRotator(0.0f)));
	Zones.Add(ZonePos::D, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation() + FVector(0.0f, -aY * aUnitSize, 0.0f), FRotator(0.0f)));
	Zones.Add(ZonePos::L, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation() + FVector(-aX * aUnitSize, 0.0f, 0.0f), FRotator(0.0f)));
	Zones.Add(ZonePos::R, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation() + FVector(aX * aUnitSize, 0.0f, 0.0f), FRotator(0.0f)));
	Zones.Add(ZonePos::UL, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation() + FVector(-aX * aUnitSize, aY * aUnitSize, 0.0f),  FRotator(0.0f)));
	Zones.Add(ZonePos::UR, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation() + FVector(aX * aUnitSize, aY * aUnitSize, 0.0f), FRotator(0.0f)));
	Zones.Add(ZonePos::DL, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation() + FVector(-aX * aUnitSize, -aY * aUnitSize, 0.0f), FRotator(0.0f)));
	Zones.Add(ZonePos::DR, world->SpawnActor<AZoneManager>(AZoneManager::StaticClass(), GetActorLocation() + FVector(aX * aUnitSize, -aY * aUnitSize, 0.0f), FRotator(0.0f)));

	// Now set them up!
	for (auto& Elem : Zones)
	{
		Elem.Value->SetupZone(ZoneOffsets[Elem.Key], aX, aY, aUnitSize, aMaterial, aFloor, aPersistence, aFrequency, aAmplitude, aOctaves, aRandomseed);
	}

}

