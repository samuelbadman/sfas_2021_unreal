// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UnrealSFASGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API UUnrealSFASGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UUnrealSFASGameInstance();

	void SetNumberOfPlayers(int Number);
	FORCEINLINE int GetNumberOfPlayers() const { return NumberOfPlayers; }

private:
	int NumberOfPlayers;
};
