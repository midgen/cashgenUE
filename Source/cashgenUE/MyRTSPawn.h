// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MyRTSPawn.generated.h"

UCLASS()
class CASHGENUE_API AMyRTSPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyRTSPawn(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* MySpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	UCameraComponent* MyCamera;

private:
	void MoveForward(float AAxisValue);
	void MoveRight(float AAxisValue);

	void ZoomIn();
	void ZoomOut();
	
};
