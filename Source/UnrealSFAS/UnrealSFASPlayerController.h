// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UnrealSFASPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API AUnrealSFASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AUnrealSFASPlayerController();

	void SpawnGameOverUI(int EnemiesDefeated, int TotalDamageDealt);

	void PauseGame(bool Pause);
	void TogglePause();

	void SetPlayerIndex(int32 Index);

	FORCEINLINE class UGameUI* GetGameUI() const { return GameUI; }
	FORCEINLINE class UGameOverUserWidget* GetGameOverUI() const { return GameOverUI; }
	FORCEINLINE class UPauseUserWidget* GetPauseUI() const { return PauseUI; }
	FORCEINLINE int32 GetPlayerIndex() const { return PlayerIndex; }

public:
	/** Called when the controller possesses a pawn. */
	void OnPossess(APawn* InPawn) override;

private:
	/** Spawns game ui for the character and adds it to the player screen. */
	void SpawnGameUI(class AUnrealSFASCharacter* unrealSFASCharacter);

private:
	class UGameUI* GameUI;
	class UGameOverUserWidget* GameOverUI;
	class UPauseUserWidget* PauseUI;
	int32 PlayerIndex;

	bool Paused;
	int32 PlayerIndexWhoPaused;

	///////////////////////////////////////////////////
	/** Game user interface category */
	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Game user interface")
	TSubclassOf<class UGameUI> GameUIClass;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Game user interface")
	TSubclassOf<class UGameOverUserWidget> GameOverWidgetClass;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Game user interface")
	TSubclassOf<class UPauseUserWidget> PauseUserWidgetClass;
	///////////////////////////////////////////////////
};
