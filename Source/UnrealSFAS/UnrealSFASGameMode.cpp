// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealSFASGameMode.h"
#include "UnrealSFASCharacter.h"
#include "UObject/ConstructorHelpers.h"

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
}
