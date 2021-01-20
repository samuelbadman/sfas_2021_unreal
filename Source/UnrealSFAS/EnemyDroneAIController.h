// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyDroneAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API AEnemyDroneAIController : public AAIController
{
	GENERATED_BODY()
	
	/** AI perception **/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AiPerceptionComponent;

public:
	AEnemyDroneAIController();

	UFUNCTION(BlueprintCallable, Category = AI)
	FORCEINLINE float GetMaxShotDistance() const { return MaxShotDistance; }
	
	UFUNCTION(BlueprintCallable, Category = AI)
	FORCEINLINE int GetMinShotDamage() const { return MinShotDamage; }

	UFUNCTION(BlueprintCallable, Category = AI)
	FORCEINLINE int GetMaxShotDamage() const { return MaxShotDamage; }

protected:
	void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;

private:
	/** Bound as delegate to OnTargetPerceptionUpdate */
	UFUNCTION()
	void OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

private:
	/** Sight sense configuration */
	class UAISenseConfig_Sight* AiSightConfig;

	///////////////////////////////////////////////
	/** Drone AI category */
	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Drone AI", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTreeAsset;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Drone AI", meta = (AllowPrivateAccess = "true"))
	FName CanSeePlayerBlackboardValueName;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Drone AI", meta = (AllowPrivateAccess = "true"))
	FName TargetBlackboardValueName;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Drone AI", meta = (AllowPrivateAccess = "true"))
	float MaxShotDistance;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Drone AI", meta = (AllowPrivateAccess = "true"))
	int MinShotDamage;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = "Drone AI", meta = (AllowPrivateAccess = "true"))
	int MaxShotDamage;
	///////////////////////////////////////////////
};
