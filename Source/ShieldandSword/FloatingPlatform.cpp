// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	StartPoint = FVector(0.f);

	EndPoint = FVector(0.f);

	Speed = 2.f;

	DelayTime = 0.1f;

	bInterping = false;

}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	bInterping = false;

	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, DelayTime);

	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	if (bInterping)
	{
		Super::Tick(DeltaTime);

		FVector CurrentLocation = GetActorLocation();

		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, Speed);

		SetActorLocation(Interp);

		if (GetActorLocation() == EndPoint)
		{
			ToggleInterping();
		}

		/** It's do same work with SwapVectors function of double if
		* float DistanceTraveled = (GetActorLocation() - Startpoint);
		* if(Distance - DistanceTraveled <= 1.f)
		*{
		*	ToggleInterping();
		*	
		*	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, DelayTime);
		* 
		*	SwapVectors(StartPoint, EndPoint);
		* }
		*/
		
	}

	if (!bInterping)
	{
		Super::Tick(DeltaTime);

		FVector CurrentLocation = GetActorLocation();

		FVector Interp = FMath::VInterpTo(CurrentLocation, StartPoint, DeltaTime, Speed);

		SetActorLocation(Interp);

		if (GetActorLocation() == StartPoint)
		{
			ToggleInterping();
		}
	}


	
}

void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

/**
* 
void AFloatingPlatform::SwapVectors(FVector& VectorOne, FVector VectorTwo)
{
	FVector Tmp = VectorOne;
	VectorOne = VectorTwo;
	VectorTwo = Tmp;
}
*
*/
