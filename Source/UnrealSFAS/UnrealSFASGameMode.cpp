// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealSFASGameMode.h"
#include "UnrealSFASCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealSFASPlayerController.h"
#include "GameUI.h"
#include "DroneCharacter.h"

AUnrealSFASGameMode::AUnrealSFASGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Set default player controller class to blueprinted player controller class
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/BP_UnrealSFASPlayerController.BP_UnrealSFASPlayerController_C'"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	// Set member default values
	CurrentWaveNumber = 0;
	CurrentNumberOfEnemies = 0;
	WaveStartCooldownDuration = 5.f;
	WaveNotificationDisplayDuration = 2.f;

	EnemySpawnVolume = nullptr;
	EnemySpawnVolumeClass = nullptr;
	EnemySpawnVolumeCenterLocation = FVector::ZeroVector;
	EnemySpawnVolumeExtent = FVector(32.f, 32.f, 32.f);
	EnemyCharacterClass = nullptr;

	WaveStartSound = nullptr;
	WaveCompleteSound = nullptr;
	BackgroundMusic = nullptr;
}

void AUnrealSFASGameMode::NotifyDroneDestroyed()
{
	// Remove an enemy from the wave.
	CurrentNumberOfEnemies--;

	// Check if all of the enemies in the wave have been defeated.
	if (CurrentNumberOfEnemies == 0)
	{
		// Call the wave complete event.
		OnWaveComplete();

		StartNextWave();
	}
}

void AUnrealSFASGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto* world = GetWorld();
	// Check the world is valid.
	if (world)
	{
		// Check the enemy spawn volume class has been set.
		if (EnemySpawnVolumeClass)
		{
			// Spawn the enemy spawn volume at the requested location.
			EnemySpawnVolume = world->SpawnActor<ASpawnVolume>(EnemySpawnVolumeClass.Get(), EnemySpawnVolumeCenterLocation, FRotator::ZeroRotator);

			// Check the enemy spawned correctly.
			if (EnemySpawnVolume)
			{
				// Setup enemy spawn volume.
				auto* enemySpawnVolumeBox = EnemySpawnVolume->GetVolume();
				enemySpawnVolumeBox->SetBoxExtent(EnemySpawnVolumeExtent);

				StartNextWave();
			}
		}

		// Play the background music.
		if (BackgroundMusic)
		{
			UGameplayStatics::PlaySound2D(world, BackgroundMusic);
		}
	}
}

void AUnrealSFASGameMode::StartWave(int WaveNumber)
{
	// Check the world is valid.
	auto* world = GetWorld();
	if (world)
	{
		// Check a class has been set to use as the enemy character.
		if (EnemyCharacterClass)
		{
			// Get enemy spawn volume.
			auto* enemySpawnVolumeBox = EnemySpawnVolume->GetVolume();

			int numberToSpawn = GetTotalNumberOfEnemiesInWave(WaveNumber);
			CurrentNumberOfEnemies = 0;

			// For each enemy in the wave.
			for (int i = 0; i < numberToSpawn; i++)
			{
				// Find a random location in the enemy spawn volume to spawn an enemy.
				FVector randomLoc = UKismetMathLibrary::RandomPointInBoundingBox(enemySpawnVolumeBox->GetComponentLocation(), enemySpawnVolumeBox->GetUnscaledBoxExtent());

				// Spawn the enemy at the found location.
				auto* spawnedEnemy = world->SpawnActor<ADroneCharacter>(EnemyCharacterClass.Get(), randomLoc, FRotator::ZeroRotator);

				// Check the enemy was spawned succesfully.
				if (spawnedEnemy)
				{
					CurrentNumberOfEnemies++;

					// Add additional hitpoints based on wave number.
					spawnedEnemy->AddHitpoints(CalculateAdditionalEnemyHitpointsForWave(WaveNumber));
				}
			}

			// Update UI wave label.
			auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, 0));
			auto* GameUI = unrealSFASPlayerController->GetGameUI();
			GameUI->SetWaveNumber(WaveNumber);

			// Call the wave started event.
			OnWaveStart();
		}
	}
}

int AUnrealSFASGameMode::GetTotalNumberOfEnemiesInWave(int WaveNumber)
{
	const int baseEnemyNumber = 1;
	return baseEnemyNumber + (2 * WaveNumber);
}

void AUnrealSFASGameMode::StartNextWave()
{
	// Bind StartWave and int parameter to timer delegate.
	WaveStartCooldownTimerDelegate.BindUFunction(this, FName("StartWave"), ++CurrentWaveNumber);

	// Set the timer to begin the wave start cooldown.
	GetWorldTimerManager().SetTimer(WaveStartCooldownTimerHandle, WaveStartCooldownTimerDelegate, WaveStartCooldownDuration, false);
}

void AUnrealSFASGameMode::OnWaveComplete()
{
	// Check the world is valid.
	auto* world = GetWorld();
	if (world)
	{
		// Play the wave complete sound.
		if (WaveCompleteSound)
		{
			UGameplayStatics::PlaySound2D(world, WaveCompleteSound);
		}

		// Notify the wave has started.
		auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, 0));
		unrealSFASPlayerController->GetGameUI()->ShowWaveStatusNotification(CurrentWaveNumber, true);

		// Begin timer to hide notification.
		GetWorldTimerManager().SetTimer(WaveNotificationTimerHandle, this, &AUnrealSFASGameMode::OnNotificationExpired, WaveNotificationDisplayDuration, false);
	}
}

void AUnrealSFASGameMode::OnWaveStart()
{
	// Check the world is valid.
	auto* world = GetWorld();
	if (world)
	{
		// Play the wave start sound.
		if (WaveStartSound)
		{
			UGameplayStatics::PlaySound2D(world, WaveStartSound);
		}

		// Notify the wave has been completed.
		auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, 0));
		unrealSFASPlayerController->GetGameUI()->ShowWaveStatusNotification(CurrentWaveNumber, false);

		// Begin timer to hide notification.
		GetWorldTimerManager().SetTimer(WaveNotificationTimerHandle, this, &AUnrealSFASGameMode::OnNotificationExpired, WaveNotificationDisplayDuration, false);
	}
}

void AUnrealSFASGameMode::OnNotificationExpired()
{
	// Check the world is valid.
	auto* world = GetWorld();
	if (world)
	{
		// Hide the notifcation widget.
		auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, 0));
		unrealSFASPlayerController->GetGameUI()->HideWaveStatusNotification();
	}
}

int AUnrealSFASGameMode::CalculateAdditionalEnemyHitpointsForWave(int WaveNumber)
{
	int baseAdditionalDroneHitpoints = 5;
	return baseAdditionalDroneHitpoints* WaveNumber;
}
