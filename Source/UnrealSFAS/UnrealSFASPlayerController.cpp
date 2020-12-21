// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSFASPlayerController.h"
#include "UnrealSFASPlayerCameraManager.h"

AUnrealSFASPlayerController::AUnrealSFASPlayerController()
{
	// set default player camera manager class to Blueprinted player camera manager
	static ConstructorHelpers::FClassFinder<APlayerCameraManager> PlayerCameraManagerBPClass(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/BP_UnrealSFASPlayerCameraManager.BP_UnrealSFASPlayerCameraManager_C'"));
	if (PlayerCameraManagerBPClass.Class != NULL)
	{
		PlayerCameraManagerClass = PlayerCameraManagerBPClass.Class;
	}
}
