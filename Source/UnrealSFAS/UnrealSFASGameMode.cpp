// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealSFASGameMode.h"
#include "UnrealSFASCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	EnemySpawnVolume = nullptr;
	EnemySpawnVolumeClass = nullptr;
	EnemySpawnVolumeCenterLocation = FVector::ZeroVector;
	EnemySpawnVolumeExtent = FVector(32.f, 32.f, 32.f);
	EnemyCharacterClass = nullptr;
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

				// Start wave 1.
				StartWave(CurrentWaveNumber++);
			}
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

			// Find a random location in the enemy spawn volume to spawn an enemy.
			FVector randomLoc = UKismetMathLibrary::RandomPointInBoundingBox(enemySpawnVolumeBox->GetComponentLocation(), enemySpawnVolumeBox->GetUnscaledBoxExtent());

			// Spawn the enemy at the found location.
			world->SpawnActor<ACharacter>(EnemyCharacterClass.Get(), randomLoc, FRotator::ZeroRotator);
		}
	}
}
