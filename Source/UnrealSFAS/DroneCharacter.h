// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DroneCharacter.generated.h"

UCLASS()
class UNREALSFAS_API ADroneCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	USceneComponent* MuzzleFlashScene;

public:
	// Sets default values for this character's properties
	ADroneCharacter();

	/** Damages the drone character, reducing its hitpoints value. Returns whether the damage destroyed the drone character. Can be extended by derived blueprint */
	bool RecieveDamage(int Amount);

	/** Returns the muzzle flash scene component. */
	FORCEINLINE USceneComponent* GetMuzzleFlashScene() const { return MuzzleFlashScene; }

	/** Returns the emitter template to use for muzzle flash. */
	UFUNCTION(BlueprintCallable, Category = Particles)
	FORCEINLINE UParticleSystem* GetMuzzleFlashEmitterTemplate() const { return MuzzleFlashEmitterTemplate; }

private:
	UPROPERTY(BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	int hitpoints;

	///////////////////////////////////////////////
	/** Particles category */
	/** The particle emitter template to use to create a muzzle flash effect when the drone fires. */
	UPROPERTY(EditDefaultsOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlashEmitterTemplate;

	/** The particle emitter template to use to play an explosion. */
	UPROPERTY(EditDefaultsOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ExplosionEmitterTemplate;

	/** Audio category */
	/** The sound to play when the drone is shot. */
	UPROPERTY(EditDefaultsOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	USoundBase* BulletImpactSound;

	/** The sound to play when the drone is destroyed. */
	UPROPERTY(EditDefaultsOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	USoundBase* DestroyedSound;
	///////////////////////////////////////////////
};
