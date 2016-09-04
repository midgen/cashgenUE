// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PawnMovementComponent.h"
#include "TestPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CASHGENUE_API UTestPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
};
