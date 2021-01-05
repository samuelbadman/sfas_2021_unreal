// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuUserWidget.h"
#include "Kismet/GameplayStatics.h"
//#include "GenericPlatform/GenericPlatformMisc.h"

void UMainMenuUserWidget::OnStartGameButtonPressed()
{
	// Open the main game level.
	UGameplayStatics::OpenLevel(GetWorld(), GameLevelName);
}

void UMainMenuUserWidget::OnQuitGameButtonPressed()
{
	// Check world is valid.
	auto* world = GetWorld();
	if (world)
	{
		// Quit the game.
		UKismetSystemLibrary::QuitGame(world, UGameplayStatics::GetPlayerController(world, 0), EQuitPreference::Quit, true);
		//FGenericPlatformMisc::RequestExit(false);
	}
}
