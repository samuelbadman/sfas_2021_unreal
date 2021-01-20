// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"
#include "MainMenuUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../UnrealSFASGameInstance.h"
#include "MainMenuGameMode.h"

AMainMenuPlayerController::AMainMenuPlayerController()
{
	// Set default class members.
	MainMenuWidgetClass = nullptr;
	PlayerIndex = 0;
	SecondPlayerJoined = false;
	InHowToPlaySection = false;
}

UMainMenuUserWidget* AMainMenuPlayerController::SpawnMainMenuUI()
{
	// Spawn main menu user interface widget
	// Check the main menu ui class is valid
	if (MainMenuWidgetClass)
	{
		auto* mainMenuWidget = CreateWidget<UMainMenuUserWidget>(this, MainMenuWidgetClass);

		// Check main menu ui widget created correctly
		if (mainMenuWidget)
		{
			// Show the ui
			mainMenuWidget->AddToViewport();

			// Set input mode to game only
			SetInputMode(FInputModeGameOnly());

			return mainMenuWidget;
		}
	}

	return nullptr;
}

void AMainMenuPlayerController::SetPlayerIndex(int32 Index)
{
	PlayerIndex = Index;
}

void AMainMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	InputComponent->BindAction("PlayerJoin", IE_Pressed, this, &AMainMenuPlayerController::PlayerJoinPressed);
	InputComponent->BindAction("PositiveMenu", IE_Pressed, this, &AMainMenuPlayerController::PositiveMenuButtonPressed);
	InputComponent->BindAction("NegativeMenu", IE_Pressed, this, &AMainMenuPlayerController::ExitGame);
}

void AMainMenuPlayerController::PlayerJoinPressed()
{
	// Is it player 2 pressing join button?
	if (PlayerIndex == 1)
	{
		// Do not allow second player to join if the menu is displaying the how to play section.
		if (!InHowToPlaySection)
		{
			SecondPlayerJoined = !SecondPlayerJoined;

			auto* world = GetWorld();
			if (world)
			{
				auto* gameInstance = CastChecked<UUnrealSFASGameInstance>(UGameplayStatics::GetGameInstance(world));
				if (SecondPlayerJoined)
				{
					gameInstance->SetNumberOfPlayers(2);

					auto* mainMenuMode = CastChecked<AMainMenuGameMode>(UGameplayStatics::GetGameMode(world));
					auto* secondPlayerActor = mainMenuMode->GetSecondPlayerActor();

					if (secondPlayerActor)
					{
						secondPlayerActor->SetActorHiddenInGame(false);
					}
				}
				else
				{
					gameInstance->SetNumberOfPlayers(1);

					auto* mainMenuMode = CastChecked<AMainMenuGameMode>(UGameplayStatics::GetGameMode(world));
					auto* secondPlayerActor = mainMenuMode->GetSecondPlayerActor();

					if (secondPlayerActor)
					{
						secondPlayerActor->SetActorHiddenInGame(true);
					}
				}
			}
		}
	}
}

void AMainMenuPlayerController::PositiveMenuButtonPressed()
{
	if (PlayerIndex == 1)
	{
		if (!HasPlayerTwoJoined()) return;
	}

	if (InHowToPlaySection)
	{
		BeginGame();
	}
	else
	{
		auto* world = GetWorld();
		if (world)
		{
			auto* mainMenuGameMode = CastChecked<AMainMenuGameMode>(UGameplayStatics::GetGameMode(world));
			auto* mainMenuWidget = mainMenuGameMode->GetMainMenuWidget();
			if (mainMenuWidget)
			{
				mainMenuWidget->ShowHowToPlaySection();
				for (int i = 0; i < 2; i++)
				{
					auto* mainMenuPC = CastChecked<AMainMenuPlayerController>(UGameplayStatics::GetPlayerController(world, i));
					mainMenuPC->InHowToPlaySection = true;
				}
			}
		}
	}
}

void AMainMenuPlayerController::BeginGame()
{
	// Remove the second player.
	UGameplayStatics::RemovePlayer(UGameplayStatics::GetPlayerController(GetWorld(), 1), true);

	// Open the main game level.
	const FName MainGameLevelName = "MazeBase";
	UGameplayStatics::OpenLevel(GetWorld(), MainGameLevelName);
}

void AMainMenuPlayerController::ExitGame()
{
	if (PlayerIndex == 1)
	{
		if (!HasPlayerTwoJoined()) return;
	}

	// Check world is valid.
	auto* world = GetWorld();
	if (world)
	{
		// Quit the game.
		UKismetSystemLibrary::QuitGame(world, UGameplayStatics::GetPlayerController(world, 0), EQuitPreference::Quit, true);
	}
}

bool AMainMenuPlayerController::HasPlayerTwoJoined() const
{
	auto* world = GetWorld();
	if (world)
	{
		auto* unrealGameInstance = CastChecked<UUnrealSFASGameInstance>(UGameplayStatics::GetGameInstance(world));
		return static_cast<bool>(unrealGameInstance->GetNumberOfPlayers() - 1);
	}

	return false;
}
