// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSFASPlayerController.h"
#include "UnrealSFASPlayerCameraManager.h"
#include "GameUI.h"

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
}

void AUnrealSFASPlayerController::BeginPlay()
{
	// Spawn game user interface
	// Check the game ui class is valid
	if (GameUIClass)
	{
		GameUI = CreateWidget<UGameUI>(this, GameUIClass);

		// Check game ui widget created correctly
		if (GameUI)
		{
			GameUI->AddToViewport();
			SetInputMode(FInputModeGameOnly());
		}
	}
}
