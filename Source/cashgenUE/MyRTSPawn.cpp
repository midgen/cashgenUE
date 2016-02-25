// Fill out your copyright notice in the Description page of Project Settings.

#include "cashgenUE.h"
#include "MyRTSPawn.h"


// Sets default values
AMyRTSPawn::AMyRTSPawn(const FObjectInitializer& ObjectInitializer)
{

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	MySpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MySpringArm"));
	MyCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MyCamera"));
	MySpringArm->AttachTo(RootComponent);
	MySpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(-80.0f, 0.0f, 0.0f));
	MySpringArm->TargetArmLength = 1500.0f;
	MySpringArm->bEnableCameraLag = true;
	MySpringArm->CameraLagSpeed = 3.0f;

	MySpringArm->bDoCollisionTest = false;

	MyCamera->AttachTo(MySpringArm, USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void AMyRTSPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMyRTSPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AMyRTSPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AMyRTSPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyRTSPawn::MoveRight);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &AMyRTSPawn::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &AMyRTSPawn::ZoomOut);
}

void AMyRTSPawn::ZoomIn()
{
	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, -100.0f));
}

void AMyRTSPawn::ZoomOut()
{
	SetActorLocation(GetActorLocation() + FVector(0.0f, 0.0f, 100.0f));
}

void AMyRTSPawn::MoveForward(float AAxisValue)
{
	if (AAxisValue != 0.0f) {
		FVector deltaMove = AAxisValue * GetActorRotation().Vector() * 20.0f;
		SetActorLocation(GetActorLocation() + deltaMove);
	}
}

void AMyRTSPawn::MoveRight(float AAxisValue)
{
	if (AAxisValue != 0.0f) {
		FVector deltaMove = AAxisValue * GetActorRotation().Vector().RightVector * 20.0f;
		SetActorLocation(GetActorLocation() + deltaMove);
	}
}