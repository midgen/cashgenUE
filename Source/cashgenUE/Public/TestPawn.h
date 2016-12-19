// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "TestPawn.generated.h"

UCLASS()
class CASHGENUE_API ATestPawn : public APawn
{
	GENERATED_BODY()

	class UTestPawnMovementComponent* MovementComponent;


public:
	// Sets default values for this pawn's properties
	ATestPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* aInputComponent) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;
	
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUp(float AxisValue);
};
