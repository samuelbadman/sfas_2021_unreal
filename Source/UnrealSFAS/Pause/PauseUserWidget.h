// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API UPauseUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Show(bool Show);

	UFUNCTION(BlueprintCallable)
	void OnResumeGameButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnExitToMainMenuButtonClicked();

private:
	/** Set in the derived blueprint. */
	UPROPERTY(EditDefaultsOnly, Category = "Game Over UI", meta = (AllowPrivateAccess = "true"))
	FName MainMenuLevelName;
};
