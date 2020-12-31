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

	FORCEINLINE class UGameUI* GetGameUI() const { return GameUI; }

private:
	class UGameUI* GameUI;

	///////////////////////////////////////////////////
	/** Game user interface category */
	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Game user interface")
	TSubclassOf<class UGameUI> GameUIClass;
	///////////////////////////////////////////////////
};
