// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprint UMETA(DisplayName = "Sprint"),

	EMS_MAX UMETA(DisplayName = "MAX"),

};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_Recovering UMETA(DisplayName = "Recovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class SHIELDANDSWORD_API AMain : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AMain();

	/**
	*
	* TArray
	*
	*/
	/*
	TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocaitons();
	*/

	/**
	*
	* Enum
	*
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	// Set movement status and running speed
	void SetMovementStatus(EMovementStatus NewStatus);

	FORCEINLINE void SetStaminaStatus(EStaminaStatus NewStaminaStatus) { StaminaStatus = NewStaminaStatus; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	// Pressed down to enable sprinting
	void ShiftKeyDown();

	// Released to stop sprinting
	void ShiftKeyUp();

	/**
	*
	* Components Settings
	*
	*/

	// Positioning the camera behind the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* CameraBoom;
	
	// It is the camera that follows the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;

	// Base turn rate and look up rate to scale turning functions for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/**
	*
	* Player Stats
	* 
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerStats")
	int32 Coins;

	void DecrementHealth(float Amounth);

	void Die();

	void DecrementCoin(int32 Amounth);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called for forwards or backwards input
	void MoveForward(float Value);

	// Called for side to side input
	void MoveRight(float Value);

	/** Called via input to turn at a given rate
	* @param Rate This is a normalized rate,
	* i.e. 1.0 means %100 of desired turn rate 
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up or down at a given rate
	* @param Rate This is a normalized rate,
	* i.e. 1.0 means %100 of desired look up or down rate 
	*/
	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetCamera() const { return FollowCamera; }

};
