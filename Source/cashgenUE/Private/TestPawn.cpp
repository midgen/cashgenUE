// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "TestPawnMovementComponent.h"
#include "TestPawn.h"


// Sets default values
ATestPawn::ATestPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	// Use a spring arm to give the camera smooth, natural-feeling motion.
	USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->RelativeRotation = FRotator(-10.f, 0.f, 0.f);
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	// Create a camera and attach to our spring arm
	UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	

	// Take control of the default player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	MovementComponent = CreateDefaultSubobject<UTestPawnMovementComponent>(TEXT("CustomMovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void ATestPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ATestPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &ATestPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATestPawn::MoveRight);
	InputComponent->BindAxis("Turn", this, &ATestPawn::Turn);
	InputComponent->BindAxis("LookUp", this, &ATestPawn::LookUp);

}

UPawnMovementComponent* ATestPawn::GetMovementComponent() const
{
	return MovementComponent;
}

void ATestPawn::MoveForward(float AxisValue)
{
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
	}
}

void ATestPawn::MoveRight(float AxisValue)
{
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
	}
}

void ATestPawn::Turn(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += AxisValue;
	SetActorRotation(NewRotation);
}

void ATestPawn::LookUp(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Pitch += AxisValue * -1.f;
	SetActorRotation(NewRotation);
}