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

	/** Handles updating the game state when a player is defeated. */
	void OnPlayerDefeated();

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

	/** Event called when a wave is completed. Used to handle events that trigger at the end of a wave: ui updates etc... */
	void OnWaveComplete();

	/** Event called when a wave starts. Used to handle events that trigger at the start of a wave: ui updates etc...*/
	void OnWaveStart();

	/** Called when a wave status notification has expired. Hides the appropriate UI widget. */
	void OnNotificationExpired();

	int CalculateAdditionalEnemyHitpointsForWave(int WaveNumber);

private:
	int CurrentWaveNumber;
	int CurrentNumberOfEnemies;
	float WaveStartCooldownDuration;

	FTimerDelegate WaveStartCooldownTimerDelegate;
	FTimerHandle WaveStartCooldownTimerHandle;

	float WaveNotificationDisplayDuration;
	FTimerHandle WaveNotificationTimerHandle;

private:
	/** The spawn volume instance used to spawn enemies in. */
	class ASpawnVolume* EnemySpawnVolume;

	/** The number of players left remaining in the game. */
	int PlayersRemaining;

	/** The number of players who started the game. */
	int StartingNumberOfPlayers;

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
	/** Audio category */
	/** Set in the derived blueprint. The sound to play at the start of a wave. */
	UPROPERTY(EditDefaultsOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	USoundBase* WaveStartSound;

	/** Set in the derived blueprint. The sound to play at the end of a wave. */
	UPROPERTY(EditDefaultsOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	USoundBase* WaveCompleteSound;

	/** Set in the derived blueprint. The background music sound. */
	UPROPERTY(EditDefaultsOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	USoundBase* BackgroundMusic;
	//////////////////////////////////
};



