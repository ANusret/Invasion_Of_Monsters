// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Main.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatComponenet"));
	CombatComponent->SetupAttachment(GetRootComponent());

	bWeaponParticle = false;

	WeaponState = EWeaponState::EWS_Pickup;

	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatComponentOverlapBegin);
	CombatComponent->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatComponentOverlapEnd);

	CombatComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor,  OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	// UE_LOG(LogTemp, Warning, TEXT("WEAPON Overlap Begin"));
	if (WeaponState == EWeaponState::EWS_Pickup && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);

		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}

}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	// UE_LOG(LogTemp, Warning, TEXT("WEAPON Overlap END"));
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);

		}
	}
}

void AWeapon::CombatComponentOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->Blood)
			{
				const USkeletalMeshSocket* SwordSocket = SkeletalMesh->GetSocketByName("SwordSocket");
				if (SwordSocket)
				{
					FVector SocketLocation = SwordSocket->GetSocketLocation(SkeletalMesh);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->Blood, SocketLocation, FRotator(0.f), false);
				}
				
			}
			if (Enemy->EnemyHitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->EnemyHitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, Damage, WeaponController, this, DamageTypeClass);
			}
		}
	}
}

void AWeapon::CombatComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::Equip(AMain* Character)
{
	if (Character)
	{
		SetWeaponController(Character->GetController());

		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);
		const USkeletalMeshSocket* RightHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");

		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Character->GetMesh());
			ParticleRotate = false;


			Character->SetEquippedWeapon(this);
			Character->SetActiveOverlappingItem(nullptr);
		}
		if (EquipSound)
		{
			UGameplayStatics::PlaySound2D(this, EquipSound);
		}
		if (!bWeaponParticle)
		{
			IdleParticleComponent->Deactivate();
		}
	}
}

void AWeapon::ActivateCollision()
{
	CombatComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CombatComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

