// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverUserWidget.h"
#include "Kismet/GameplayStatics.h"

void UGameOverUserWidget::OnReturnToMenuClicked()
{
	// Open the main menu level.
	UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
}
