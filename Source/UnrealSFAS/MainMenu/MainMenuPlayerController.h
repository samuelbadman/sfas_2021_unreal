// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainMenuPlayerController();

protected:
	void BeginPlay() override;

private:
	/** Set in the derived blueprint. */
	UPROPERTY(EditDefaultsOnly, Category = MainMenu)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
};
