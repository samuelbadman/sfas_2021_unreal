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
	CurrentWaveNumber = 1;

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
	if (world)
	{
		EnemySpawnVolume = world->SpawnActor<ASpawnVolume>(EnemySpawnVolumeClass.Get(), EnemySpawnVolumeCenterLocation, FRotator::ZeroRotator);

		if (EnemySpawnVolume)
		{
			auto* volumeBox = EnemySpawnVolume->GetVolume();
			volumeBox->SetBoxExtent(EnemySpawnVolumeExtent);

			FVector randomLoc = UKismetMathLibrary::RandomPointInBoundingBox(volumeBox->GetComponentLocation(), volumeBox->GetUnscaledBoxExtent());
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("%s"), *randomLoc.ToString()));

			if (EnemyCharacterClass)
			{
				world->SpawnActor<ACharacter>(EnemyCharacterClass.Get(), randomLoc, FRotator::ZeroRotator);
			}
		}
	}
}
