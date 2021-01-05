// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "MainMenuPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	// Set main menu player controller derived blueprint.
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/BP_MainMenuPlayerController.BP_MainMenuPlayerController_C'"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	// Remove default pawn.
	DefaultPawnClass = nullptr;
}
