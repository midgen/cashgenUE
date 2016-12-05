// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Classes/Components/HierarchicalInstancedStaticMeshComponent.h"
#include "CGFoliageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CASHGEN_API UCGFoliageComponent : public UActorComponent
{
	GENERATED_BODY()

	bool UCGFoliageComponent::GetGodCastHitPos(const FVector aVectorToStart, FVector* aHitPos, FVector* aNormalVector);

	UHierarchicalInstancedStaticMeshComponent* HISM;

	const int32 MAX_INSTANCES = 1024;
	int32 numInstances = 0;

public:	
	// Sets default values for this component's properties
	UCGFoliageComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

		
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* StaticMesh;
};
