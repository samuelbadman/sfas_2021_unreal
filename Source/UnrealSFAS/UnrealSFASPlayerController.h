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

	void BeginPlay() override;

	void SpawnGameOverUI(int EnemiesDefeated, int TotalDamageDealt);

	void PauseGame(bool Pause);
	void TogglePause();

	FORCEINLINE class UGameUI* GetGameUI() const { return GameUI; }
	FORCEINLINE class UPauseUserWidget* GetPauseUI() const { return PauseUI; }

private:
	class UGameUI* GameUI;
	class UGameOverUserWidget* GameOverUI;
	class UPauseUserWidget* PauseUI;

	bool Paused;

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
