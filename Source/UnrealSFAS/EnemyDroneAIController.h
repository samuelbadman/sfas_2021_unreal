// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyDroneAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API AEnemyDroneAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyDroneAIController();

protected:
	void BeginPlay() override;

private:
	// Set in the derived blueprint
	UPROPERTY(EditDefaultsOnly, Category = "Drone AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTreeAsset;
};
