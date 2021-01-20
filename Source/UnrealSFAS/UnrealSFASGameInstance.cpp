// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSFASGameInstance.h"

UUnrealSFASGameInstance::UUnrealSFASGameInstance()
{
	// Set default member values
	NumberOfPlayers = 1;
}

void UUnrealSFASGameInstance::SetNumberOfPlayers(int Number)
{
	NumberOfPlayers = Number;
}
