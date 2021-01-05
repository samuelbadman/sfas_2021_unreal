// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSFASPlayerController.h"
#include "UnrealSFASPlayerCameraManager.h"
#include "GameUI.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealSFASGameMode.h"
#include "GameOver/GameOverUserWidget.h"
#include "Pause/PauseUserWidget.h"

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
	PauseUI = nullptr;
	PauseUserWidgetClass = nullptr;
	Paused = false;
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

	// Check the pause ui class is valid.
	if (PauseUserWidgetClass)
	{
		PauseUI = CreateWidget<UPauseUserWidget>(this, PauseUserWidgetClass);

		// Check the pause ui widget created correctly
		if (PauseUI)
		{
			// Add the UI.
			PauseUI->AddToViewport();

			// Hide the pause menu.
			PauseUI->Show(false);
		}
	}
}

void AUnrealSFASPlayerController::SpawnGameOverUI(int EnemiesDefeated, int TotalDamageDealt)
{
	// Spawn game over user interface
	// Check the game over ui class is valid
	if (GameOverWidgetClass)
	{
		GameOverUI = CreateWidget<UGameOverUserWidget>(this, GameOverWidgetClass);

		// Check game over UI created correctly.
		if (GameOverUI)
		{
			// Setup game over ui.
			GameOverUI->SetEnemiesDefeatedText(EnemiesDefeated);
			GameOverUI->SetDamageDealtText(TotalDamageDealt);

			// Check the world is valid.
			auto* world = GetWorld();
			if (world)
			{
				// Cast to UnrealSFASGameMode.
				auto* unrealSFASGameMode = CastChecked<AUnrealSFASGameMode>(UGameplayStatics::GetGameMode(world));
				GameOverUI->SetWavesSurvivedText(unrealSFASGameMode->GetCurrentWaveNumber() - 1);
			}

			GameOverUI->AddToViewport();
		}
	}
}

void AUnrealSFASPlayerController::PauseGame(bool Pause)
{
	Paused = Pause;

	if (Pause)
	{
		// Hide the game UI
		//GameUI->Show(false);

		// Show the pause menu
		PauseUI->Show(true);

		// Pause the game
		SetPause(true);

		// Set input mode to game and ui.
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
	}
	else
	{
		// Show the game UI
		//GameUI->Show(true);

		// Show the pause menu
		PauseUI->Show(false);

		// Resume the game
		SetPause(false);

		// Set input mode to game only.
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}

void AUnrealSFASPlayerController::TogglePause()
{
	PauseGame(!Paused);
}
