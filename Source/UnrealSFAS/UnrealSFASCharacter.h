// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnrealSFASCharacter.generated.h"

UCLASS(config=Game)
class AUnrealSFASCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** AI stimuli source */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionStimuliSourceComponent* AiStimuliSource;

public:
	AUnrealSFASCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Damages the player character's hitpoints. */
	UFUNCTION(BlueprintCallable, Category = Damage)
	void RecieveDamage(int Amount);

	FORCEINLINE int GetHitpoints() const { return Hitpoints; }

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Put's the player character into the aiming state. */
	void AimWeapon();

	/** Exit's the player character from the aiming state and returns them to their normal state. */
	void StopAimingWeapon();

	/** Fire's the player's currently equipped weapon. */
	void FireWeapon();

	/** Swaps the shoulder the player is looking over when aiming. */
	void SwapShoulder();

protected:
	/** Called on game start. */
	void BeginPlay() override;

	/** Called every tick. */
	void Tick(float DeltaTime) override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Calculates and returns the normalised offset between the actor and control pitches */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = Aim)
	float GetPitchOffset() const;

	/** Returns the current aim blend weight value */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = Aim)
	FORCEINLINE float GetAimBlendWeight() const { return AimBlendWeight; }

private:
	void SpawnWeapon();

	void UpdateCameraZoom(const float DeltaTime);
	void UpdateCameraLocationOffset(const float DeltaTime);
	void UpdateViewPitch(const float DeltaTime);

	void SwapAimingShoulder();

	void PausePressed();

	void ShowHitMarker();
	void HideHitMarker();

	/** Triggers game over state. */
	void OnPlayerDefeated();

	/** Updates the player's hitpoint value in the UI display. */
	void UpdateHitpointsUI();

private:
	float TargetBoomLength;
	float DefaultBoomLength;
	FVector TargetCameraOffset;
	float AimBlendWeight;
	float DefaultViewMinPitch;
	float DefaultViewMaxPitch;
	float DefaultMaxWalkSpeed;
	class APlayerCameraManager* CameraManager;
	float TargetViewPitchMin;
	float TargetViewPitchMax;
	float GameSecondsAtLastShot;
	class AWeapon* Weapon;
	bool Aiming;
	bool AimingOverRightShoulder;
	FVector DefaultCameraRelativeLocation;

	FTimerHandle HitMarkerTimerHandle;

	int Hitpoints; 
	bool Defeated;

	int NumberOfEnemiesDefeated;
	int DamageDealt;

	///////////////////////////////////////////////////
	/** Weapon category */
	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<class AWeapon> DefaultWeaponClass;
	///////////////////////////////////////////////////
	/** Aim category */
	UPROPERTY(EditDefaultsOnly, Category = Aim, meta = (AllowPrivateAccess = "true"))
	float AimBoomLength;

	UPROPERTY(EditDefaultsOnly, Category = Aim, meta = (AllowPrivateAccess = "true"))
	FVector CameraAimOffset;

	UPROPERTY(EditDefaultsOnly, Category = Aim, meta = (AllowPrivateAccess = "true"))
	float CameraAimMinPitch;

	UPROPERTY(EditDefaultsOnly, Category = Aim, meta = (AllowPrivateAccess = "true"))
	float CameraAimMaxPitch;

	UPROPERTY(EditDefaultsOnly, Category = Aim, meta = (AllowPrivateAccess = "true"))
	float CameraZoomSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Aim, meta = (AllowPrivateAccess = "true"))
	float CameraMoveSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Aim, meta = (AllowPrivateAccess = "true"))
	float ViewPitchAdjustSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Aim, meta = (AllowPrivateAccess = "true"))
	float AimMaxWalkSpeed;
	///////////////////////////////////////////////////
	/** UI category */
	UPROPERTY(EditDefaultsOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	float HitMarkerDisplayDuration;
	///////////////////////////////////////////////////
	/** Defeated category */
	UPROPERTY(EditDefaultsOnly, Category = Defeated, meta = (AllowPrivateAccess = "true"))
	FVector DefeatedTargetCameraOffset;

	UPROPERTY(EditDefaultsOnly, Category = Defeated, meta = (AllowPrivateAccess = "true"))
	float DefeatedTargetCameraBoomLength;
	///////////////////////////////////////////////////
};

