// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"
#include "Camera/CameraComponent.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentVelocity = FVector(0.f);

	MaxSpeed = 200.f;

	// RootComponent'i atadým
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	//MeshmComponent'i atadým
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	// MeshComponent'i RootComponent'e baðladým
	MeshComponent->SetupAttachment(GetRootComponent());

	// Camera'yý atadým
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// Camera'yý RootComponent'e baðladým
	Camera->SetupAttachment(GetRootComponent());
	// Camera konum ayarlarýný yaptým
	Camera->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	// Camera rotasyon ayarlarýný yaptým
	Camera->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	SetActorLocation(NewLocation);

}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);

}

void ACritter::MoveForward(float Value)
{
	CurrentVelocity.X = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
}

void ACritter::MoveRight(float Value)
{
	CurrentVelocity.Y = FMath::Clamp(Value, -1.f, 1.f) * MaxSpeed;
}
