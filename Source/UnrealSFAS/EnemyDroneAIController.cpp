// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDroneAIController.h"

AEnemyDroneAIController::AEnemyDroneAIController()
{
	// Set default member values
	BehaviorTreeAsset = nullptr;
}

void AEnemyDroneAIController::BeginPlay()
{
	Super::BeginPlay();

	auto* controlledPawn = GetPawn();
	// Check controlled pawn is valid.
	if (controlledPawn)
	{
		// Add enemy tag to controlled pawn actor.
		GetPawn()->Tags.Add(FName("Enemy"));
	}

	// Check behaviour tree asset is valid.
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}
