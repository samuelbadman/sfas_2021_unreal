// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSFASPlayerController.h"
#include "UnrealSFASPlayerCameraManager.h"
#include "GameUI.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealSFASGameMode.h"
#include "GameOver/GameOverUserWidget.h"

AUnrealSFASPlayerController::AUnrealSFASPlayerController()
{
	// Set default player camera manager class to Blueprinted player camera manager
	static ConstructorHelpers::FClassFinder<APlayerCameraManager> PlayerCameraManagerBPClass(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/BP_UnrealSFASPlayerCameraManager.BP_UnrealSFASPlayerCameraManager_C'"));
	if (PlayerCameraManagerBPClass.Class != NULL)
	{
		PlayerCameraManagerClass = PlayerCameraManagerBPClass.Class;
	}

	// Set member default values
	GameUI = nullptr;
	GameOverUI = nullptr;
	GameUIClass = nullptr;
	GameOverWidgetClass = nullptr;
}

void AUnrealSFASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Spawn game user interface
	// Check the game ui class is valid
	if (GameUIClass)
	{
		GameUI = CreateWidget<UGameUI>(this, GameUIClass);

		// Check game ui widget created correctly
		if (GameUI)
		{
			// Show the ui
			GameUI->AddToViewport();

			// Set input mode to only the game
			SetInputMode(FInputModeGameOnly());

			// Setup initial ui state
			auto* gameMode = CastChecked<AUnrealSFASGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			GameUI->SetWaveNumber(gameMode->GetCurrentWaveNumber());
		}
	}
}

void AUnrealSFASPlayerController::SpawnGameOverUI()
{
	// Spawn game over user interface
	// Check the game over ui class is valid
	if (GameOverWidgetClass)
	{
		GameOverUI = CreateWidget<UGameOverUserWidget>(this, GameOverWidgetClass);

		// Check game over UI created correctly.
		if (GameOverUI)
		{
			GameOverUI->AddToViewport();
		}
	}
}
