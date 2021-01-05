// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../UnrealSFASPlayerController.h"

void UPauseUserWidget::OnResumeGameButtonClicked()
{
	auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	unrealSFASPlayerController->PauseGame(false);
}

void UPauseUserWidget::OnExitToMainMenuButtonClicked()
{
	// Open the main menu level.
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
}
