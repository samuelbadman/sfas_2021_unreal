// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDroneAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyDroneAIController::AEnemyDroneAIController()
{
	// Setup AI sight perception configuration
	AiSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AIConfigSight"));
	AiSightConfig->PeripheralVisionAngleDegrees = 75.f;
	AiSightConfig->SightRadius = 2000.f;
	AiSightConfig->LoseSightRadius = 3000.f;
	AiSightConfig->DetectionByAffiliation.bDetectEnemies = true;
	AiSightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AiSightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Setup AI perception component.
	AiPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	AiPerceptionComponent->ConfigureSense(*AiSightConfig);
	AiPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	AiPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyDroneAIController::OnPerceptionUpdate);

	// Set default member values
	BehaviorTreeAsset = nullptr;
	MaxShotDistance = 1000.f;
	MinShotDamage = 1;
	MaxShotDamage = 5;
}

void AEnemyDroneAIController::BeginPlay()
{
	Super::BeginPlay();

	// Check behaviour tree asset is valid.
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AEnemyDroneAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Add enemy tag to controlled pawn actor.
	InPawn->Tags.Add(FName("Enemy"));
}

void AEnemyDroneAIController::OnPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	// Does the perceived actor have the "Player" tag?
	if (Actor->ActorHasTag(FName("Player")))
	{
		// Did the player enter or leave the AI's sight?
		if (Stimulus.WasSuccessfullySensed())
		{
			// Set can see player to true
			GetBlackboardComponent()->SetValueAsBool(CanSeePlayerBlackboardValueName, true);

			// Set reference to the seen object
			GetBlackboardComponent()->SetValueAsObject(TargetBlackboardValueName, Actor);
		}
		else
		{
			// Set can see player to false
			GetBlackboardComponent()->SetValueAsBool(CanSeePlayerBlackboardValueName, false);

			// Set target to null
			GetBlackboardComponent()->SetValueAsObject(TargetBlackboardValueName, nullptr);
		}
	}
}
