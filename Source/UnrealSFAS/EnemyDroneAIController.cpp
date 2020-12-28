// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDroneAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyDroneAIController::AEnemyDroneAIController()
{
	// Setup AI sight perception configuration
	AiConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AIConfigSight"));
	AiConfigSight->DetectionByAffiliation.bDetectEnemies = true;
	AiConfigSight->DetectionByAffiliation.bDetectFriendlies = true;
	AiConfigSight->DetectionByAffiliation.bDetectNeutrals = true;

	// Setup AI perception component.
	AiPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	AiPerceptionComponent->ConfigureSense(*AiConfigSight);
	AiPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	AiPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyDroneAIController::OnPerceptionUpdate);

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

void AEnemyDroneAIController::OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	// Does the perceived actor have the "Player" tag?
	if (Actor->ActorHasTag(FName("Player")))
	{
		GetBlackboardComponent()->SetValueAsBool(CanSeePlayerBlackboardValueName, true);
		UE_LOG(LogTemp, Warning, TEXT("Seen player"));
	}
}
