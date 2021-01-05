// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"
#include "MainMenuUserWidget.h"

AMainMenuPlayerController::AMainMenuPlayerController()
{
	// Set default class members.
	MainMenuWidgetClass = nullptr;
}

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

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

			// Set input mode to ui only
			SetInputMode(FInputModeUIOnly());
		}
	}

	bShowMouseCursor = true;
}
