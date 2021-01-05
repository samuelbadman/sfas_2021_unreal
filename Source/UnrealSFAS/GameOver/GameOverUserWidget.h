// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API UGameOverUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Game Over UI")
	void OnReturnToMenuClicked();

	UFUNCTION(BlueprintImplementableEvent)
	void SetEnemiesDefeatedText(int EnemiesDefeated);

	UFUNCTION(BlueprintImplementableEvent)
	void SetWavesSurvivedText(int WavesSurvived);

	UFUNCTION(BlueprintImplementableEvent)
	void SetDamageDealtText(int Damage);

private:
	/** Set in the derived blueprint. */
	UPROPERTY(EditDefaultsOnly, Category = "Game Over UI", meta = (AllowPrivateAccess = "true"))
	FName MainMenuLevelName;
};
