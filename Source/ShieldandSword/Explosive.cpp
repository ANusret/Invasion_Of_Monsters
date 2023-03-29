// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


AExplosive::AExplosive() 
{
	Damage = 35.f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	// UE_LOG(LogTemp, Warning, TEXT("EXPLOSIVE Overlap Begin"));
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (OverlapParticleComponent)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticleComponent, GetActorLocation(), FRotator(0.f), true);
			}
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			Main->DecrementHealth(Damage);
			Destroy();
		}
	}
}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	// UE_LOG(LogTemp, Warning, TEXT("EXPLOSIVE Overlap END"));
}
