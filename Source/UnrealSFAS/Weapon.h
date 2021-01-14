// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class UNREALSFAS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	USceneComponent* MuzzleFlashScene;

public:	
	// Sets default values for this actor's properties
	AWeapon();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Components)
	FORCEINLINE UStaticMeshComponent* GetStaticMesh() const { return StaticMesh; }

	FORCEINLINE USceneComponent* GetMuzzeFlashScene() const { return MuzzleFlashScene; }

	FORCEINLINE class UAnimMontage* GetShotAnimMontage() const { return ShotAnimMontage; }
	FORCEINLINE float GetShotRecoverTime() const { return ShotRecoverTime; }
	FORCEINLINE float GetShotMaxRange() const { return ShotMaxRange; }
	FORCEINLINE int GetMinDamage() const { return MinDamage; }
	FORCEINLINE int GetMaxDamage() const { return MaxDamage; }
	FORCEINLINE USoundBase* GetFireSound() const { return FireSound; }
	FORCEINLINE UParticleSystem* GetMuzzleFlashEmitterTemplate() const { return MuzzleFlashEmitterTemplate; }
	FORCEINLINE float GetMaximumDeviation() const { return MaximumDeviation; }

private:
	/** Weapon category */
	/** The montage to play when the weapon is fired. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	class UAnimMontage* ShotAnimMontage;

	/** The amount of time (seconds) before the weapon can be fired again. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float ShotRecoverTime;

	/** The maximum units the weapon can hit an object at. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float ShotMaxRange;

	/** The minimum amount of damage a shot of this weapon can do. Used as the bottom of a range. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	int MinDamage;

	/** The maximum amount of damage a shot of this weapon can do. Used as the top of a range. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	int MaxDamage;

	/** The sound to play when the weapon fires. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	USoundBase* FireSound;

	/** The particle system to spawn at the weapon's muzzle location when fired. Creates a muzzle flash effect. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	UParticleSystem* MuzzleFlashEmitterTemplate;

	/** The maximum units a shot of this weapon can deviate when fired with low accuracy. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float MaximumDeviation;
	/////////////////////////////////////////////////////////////
};
