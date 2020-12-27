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

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}
