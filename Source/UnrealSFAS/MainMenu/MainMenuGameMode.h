// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenuGameMode();

	FORCEINLINE AActor* GetSecondPlayerActor() const { return SecondPlayerActor; }
	FORCEINLINE class UMainMenuUserWidget* GetMainMenuWidget() const { return MainMenuWidget; }

protected:
	void BeginPlay() override;

private:
	AActor* SecondPlayerActor;
	class UMainMenuUserWidget* MainMenuWidget;
};
