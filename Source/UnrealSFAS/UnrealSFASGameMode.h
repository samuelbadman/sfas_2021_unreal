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

	FORCEINLINE int GetCurrentWaveNumber() const { return CurrentWaveNumber; }

protected:
	void BeginPlay() override;

private:
	void StartWave(int WaveNumber);

private:
	int CurrentWaveNumber;

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



