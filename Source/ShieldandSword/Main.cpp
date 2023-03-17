// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create CameraBoom // Pulls towards the player if there's a collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(46.f, 110.f);

	// Create FollowCamera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// Attach the camera to the end of the CameraBoom
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	/** Don't rotate when the controller rotates
	* Let that just affect the camera
	*/
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ... at this rotation rate.
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.15f;

	MaxHealth = 100.f;
	Health = 70.f;
	MaxStamina = 250.f;
	Stamina = 90.f;
	Coins = 0;
	
	RunningSpeed = 600.f;
	SprintingSpeed = 900.f;
	
	bShiftKeyDown = false;

	bDropEqiupDown = false;

	bLMBDown = false;

	// Initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 20.f;
	MinSprintStamina = 50.f;
}

void AMain::SetMovementStatus(EMovementStatus NewStatus)
{
	MovementStatus = NewStatus;
	if (MovementStatus == EMovementStatus::EMS_Sprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else if (MovementStatus == EMovementStatus::EMS_Normal)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::Attack()
{
	
	if (!bAttacking)
	{
		bAttacking = true;

		UAnimInstance* AnimationInstance = GetMesh()->GetAnimInstance();
		if (AnimationInstance && CombatMontage)
		{
			int32 Random = FMath::RandRange(0, 2);
			switch (Random)
			{
			case 0:
				AnimationInstance->Montage_Play(CombatMontage, 1.f, EMontagePlayReturnType::MontageLength, 2.3f, true);
				AnimationInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
			case 1:
				AnimationInstance->Montage_Play(CombatMontage, 1.f, EMontagePlayReturnType::MontageLength, 2.3f, true);
				AnimationInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;
			case 2:
				AnimationInstance->Montage_Play(CombatMontage, 1.f, EMontagePlayReturnType::MontageLength, 2.3f, true);
				AnimationInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
				break;
			default:
				break;
			}
			
		}

	}

}

void AMain::EndAttack()
{
	bAttacking = false;
	if (bLMBDown)
	{
		Attack();
	}
}

void AMain::DecrementHealth(float Amounth)
{
	if (Health - Amounth <= 0.f)
	{
		Health -= Amounth;
		Die();
	}
	else
	{
		Health -= Amounth;
	}
}

void AMain::Die()
{
}

void AMain::DecrementCoin(int32 Amounth)
{
	Coins += Amounth;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinSprintStamina)
			{
				Stamina -= DeltaStamina;
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}
			else
			{
				Stamina -= DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Sprint);
		}
		else // Shift key up
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0.f;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprint);
			}
		}
		else // Shift key up
		{
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				Stamina += DeltaStamina;
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else // Shift key up
		{
			Stamina += DeltaStamina;
			SetStaminaStatus(EStaminaStatus::ESS_Recovering);
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	case EStaminaStatus::ESS_Recovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			Stamina += DeltaStamina;
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
	default:
		break;
	}

}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("DropEquip", IE_Pressed, this, &AMain::DropEquipDown);
	PlayerInputComponent->BindAction("DropEquip", IE_Released, this, &AMain::DropEquipUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);


}

void AMain::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0) && (!bAttacking))
	{
		/** returns a rotator that gives you the direction 
		* that the controllers facing this frame 
		*/
		const FRotator Rotation = Controller->GetControlRotation();
		// find out which way is forward
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMain::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0) && (!bAttacking))
	{
		/** returns a rotator that gives you the direction
		* that the controllers facing this frame 
		*/
		const FRotator Rotation = Controller->GetControlRotation();
		// find out which way is forward
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::DropEquipDown()
{
	bDropEqiupDown = true;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	
}

void AMain::DropEquipUp()
{
	bDropEqiupDown = false;
}

void AMain::LMBDown()
{
	bLMBDown = true;

	
	if (EquippedWeapon)
	{
		Attack();
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::SetEquippedWeapon(AWeapon* WeaponTemp)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponTemp;
}

/*
void AMain::ShowPickupLocaitons()
{
	for (int32 i = 0; i < PickupLocations.Num(); i++)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 12, FLinearColor::White, 10.f, 1.f);
	}
}
*/