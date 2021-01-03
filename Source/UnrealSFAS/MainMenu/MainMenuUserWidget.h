// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void OnStartGameButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void OnQuitGameButtonPressed();

private:
	/** The name of the level asset to open when the game is started. Set in the derived blueprint. */
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu", meta = (AllowPrivateAccess = "true"))
	FName GameLevelName;
};
