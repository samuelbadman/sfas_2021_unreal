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
#include "UnrealSFASGameInstance.h"
#include "GameOver/GameOverUserWidget.h"

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

	StartingNumberOfPlayers = 0;
	PlayersRemaining = 0;
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

void AUnrealSFASGameMode::OnPlayerDefeated()
{
	PlayersRemaining--;

	if (PlayersRemaining == 0)
	{
		// Enable returning to the main menu.
		auto* world = GetWorld();
		if (world)
		{
			// For each player that started the game.
			for (int i = 0; i < StartingNumberOfPlayers; i++)
			{
				// Show UI prompt in game over menu.
				auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, i));
				auto* gameOverUI = unrealSFASPlayerController->GetGameOverUI();
				if (gameOverUI)
				{
					gameOverUI->ShowReturnPrompt();
				}

				// Enable returning to the main menu.
				auto* unrealSFASCharacter = CastChecked<AUnrealSFASCharacter>(UGameplayStatics::GetPlayerCharacter(world, i));
				unrealSFASCharacter->SetCanReturnToMainMenu(true);
			}
		}
	}
}

void AUnrealSFASGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto* world = GetWorld();
	// Check the world is valid.
	if (world)
	{
		// Get the game instance.
		auto* gameInstance = CastChecked<UUnrealSFASGameInstance>(UGameplayStatics::GetGameInstance(world));

		// Set players remaining to total number of players.
		StartingNumberOfPlayers = gameInstance->GetNumberOfPlayers();
		PlayersRemaining = StartingNumberOfPlayers;

		// Create an additional player if one has joined.
		if (PlayersRemaining == 2)
		{
			// Enable split screen
			world->GetGameViewport()->SetForceDisableSplitscreen(false);

			// Create and setup the second player.
			auto* unrealSFASController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::CreatePlayer(world, 1));
			unrealSFASController->SetPlayerIndex(1);
			auto* newCharacter = CastChecked<AUnrealSFASCharacter>(UGameplayStatics::GetPlayerCharacter(world, 1));
			newCharacter->SetPlayerIndex(1);
		}

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
				FActorSpawnParameters spawnParams;
				spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				auto* spawnedEnemy = world->SpawnActor<ADroneCharacter>(EnemyCharacterClass.Get(), randomLoc, FRotator::ZeroRotator, spawnParams);

				// Check the enemy was spawned succesfully.
				if (spawnedEnemy)
				{
					CurrentNumberOfEnemies++;

					// Add additional hitpoints based on wave number.
					spawnedEnemy->AddHitpoints(CalculateAdditionalEnemyHitpointsForWave(WaveNumber));
				}
			}

			// Update UI wave label for each player in the game.
			auto* unrealSFASGameInstance = CastChecked<UUnrealSFASGameInstance>(UGameplayStatics::GetGameInstance(world));
			auto numPlayers = unrealSFASGameInstance->GetNumberOfPlayers();
			for (int i = 0; i < numPlayers; i++)
			{
				auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, i));
				auto* GameUI = unrealSFASPlayerController->GetGameUI();
				GameUI->SetWaveNumber(WaveNumber);
			}

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

		// Notify each player that the wave has started.
		for (int i = 0; i < StartingNumberOfPlayers; i++)
		{
			auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, i));
			unrealSFASPlayerController->GetGameUI()->ShowWaveStatusNotification(CurrentWaveNumber, true);
		}

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

		// Notify each player that the wave has been completed.
		for (int i = 0; i < StartingNumberOfPlayers; i++)
		{
			auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, i));
			unrealSFASPlayerController->GetGameUI()->ShowWaveStatusNotification(CurrentWaveNumber, false);
		}

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
		for (int i = 0; i < StartingNumberOfPlayers; i++)
		{
			auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, i));
			unrealSFASPlayerController->GetGameUI()->HideWaveStatusNotification();
		}
	}
}

int AUnrealSFASGameMode::CalculateAdditionalEnemyHitpointsForWave(int WaveNumber)
{
	int baseAdditionalDroneHitpoints = 5;
	return baseAdditionalDroneHitpoints* WaveNumber;
}
