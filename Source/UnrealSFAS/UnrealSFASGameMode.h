// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealSFASGameMode.generated.h"

UCLASS(minimalapi)
class AUnrealSFASGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUnrealSFASGameMode();

	/** Returns the current wave the game is on. */
	FORCEINLINE int GetCurrentWaveNumber() const { return CurrentWaveNumber; }

	/** Updates the round when a drone is destroyed. */
	void NotifyDroneDestroyed();

protected:
	void BeginPlay() override;

private:
	/** Starts the wave specified. */
	UFUNCTION()
	void StartWave(int WaveNumber);

	/** Calculates the number of enemies that should be spawned at the start of a wave. */
	int GetTotalNumberOfEnemiesInWave(int WaveNumber);

	/** Increments CurrentWaveNumber and starts the wave start cooldown timer to start the next wave. */
	void StartNextWave();

private:
	int CurrentWaveNumber;
	int CurrentNumberOfEnemies;
	float WaveStartCooldownDuration;

	FTimerDelegate WaveStartCooldownTimerDelegate;
	FTimerHandle WaveStartCooldownTimerHandle;

private:
	/** The spawn volume instance used to spawn enemies in. */
	class ASpawnVolume* EnemySpawnVolume;

	//////////////////////////////////
	/** Enemies category */
	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = Enemies, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASpawnVolume> EnemySpawnVolumeClass;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = Enemies, meta = (AllowPrivateAccess = "true"))
	FVector EnemySpawnVolumeCenterLocation;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = Enemies, meta = (AllowPrivateAccess = "true"))
	FVector EnemySpawnVolumeExtent;

	/** Set in the derived blueprint. The character class to spawn as enemies. */
	UPROPERTY(EditDefaultsOnly, Category = Enemies, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ACharacter> EnemyCharacterClass;
	//////////////////////////////////
};



