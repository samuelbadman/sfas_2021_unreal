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

	class UMainMenuUserWidget* SpawnMainMenuUI();
	void SetPlayerIndex(int32 Index);

protected:
	void SetupInputComponent() override;

private:
	void PlayerJoinPressed();
	void PositiveMenuButtonPressed();
	void BeginGame();
	void ExitGame();
	
private:
	bool HasPlayerTwoJoined() const;

private:
	int32 PlayerIndex;
	bool SecondPlayerJoined;
	bool InHowToPlaySection;

	/** Set in the derived blueprint. */
	UPROPERTY(EditDefaultsOnly, Category = MainMenu)
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
};
