// Fill out your copyright notice in the Description page of Project Settings.

#include "CashGen.h"
#include "CGFoliageComponent.h"


// Sets default values for this component's properties
UCGFoliageComponent::UCGFoliageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCGFoliageComponent::BeginPlay()
{
	Super::BeginPlay();

	FString compString = "HISM";
	FName compName = FName(*compString);
	HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, compName);
	HISM->RegisterComponent();
	HISM->SetStaticMesh(StaticMesh);
	HISM->bCastDynamicShadow = true;
	HISM->CastShadow = true;
	HISM->SetHiddenInGame(false);
	HISM->SetMobility(EComponentMobility::Movable);
	HISM->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HISM->BodyInstance.SetObjectType(ECC_WorldDynamic);
	HISM->BodyInstance.SetResponseToAllChannels(ECR_Ignore);
	HISM->BodyInstance.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
	HISM->BodyInstance.SetResponseToChannel(ECC_Pawn, ECR_Ignore);
	HISM->BodyInstance.SetResponseToChannel(ECC_WorldDynamic, ECR_Block);
	
}


// Called every frame
void UCGFoliageComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (numInstances < MAX_INSTANCES)
	{
		FVector startPos = FVector(FMath::RandRange(-1000.0f, 1000.0f), FMath::RandRange(-1000.0f, 1000.0f), 50000.0f);

		FVector spawnPoint = FVector(0.0f, 0.0f, 0.0f);
		FVector normalVector = FVector(0.0f);
		if (GetGodCastHitPos(startPos, &spawnPoint, &normalVector))
		{
			FRotator rotation = FRotator(0.0f, FMath::FRandRange(-90.f, 90.0f), 0.0f);

			rotation += normalVector.Rotation() + FRotator(-90.0f, 0.0f, 0.0f);

			HISM->AddInstance(FTransform(rotation, spawnPoint, FVector(1.0f)));
			numInstances++;
		}
	}
}

bool UCGFoliageComponent::GetGodCastHitPos(const FVector aVectorToStart, FVector* aHitPos, FVector* aNormalVector)
{
	const FName TraceTag("MyTraceTag");
	FCollisionQueryParams MyTraceParams = FCollisionQueryParams(FName(TEXT("TreeTrace")), true);
	MyTraceParams.bTraceComplex = false;
	MyTraceParams.bTraceAsyncScene = true;
	MyTraceParams.bReturnPhysicalMaterial = false;

	FCollisionResponseParams MyResponseParams = FCollisionResponseParams();

	FHitResult MyHitResult(ForceInit);

	FVector MyCastDirection = FVector(0.0f, 0.0f, -1.0f);

	if (GetWorld()->LineTraceSingleByChannel(MyHitResult, aVectorToStart, aVectorToStart + (MyCastDirection * 900000.0f), ECC_GameTraceChannel1, MyTraceParams, MyResponseParams))
	{
		AActor* hitActor = MyHitResult.GetActor();
		if (hitActor)
		{
			(*aNormalVector) = MyHitResult.Normal;
			(*aHitPos) = MyHitResult.ImpactPoint;
			return true;
		}
	}

	return false;
}
