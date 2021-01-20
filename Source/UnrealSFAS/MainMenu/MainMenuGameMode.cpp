// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "MainMenuPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "../UnrealSFASGameInstance.h"
#include "Animation/SkeletalMeshActor.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	// Set main menu player controller derived blueprint.
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("Blueprint'/Game/ThirdPersonCPP/Blueprints/BP_MainMenuPlayerController.BP_MainMenuPlayerController_C'"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	// Remove default pawn.
	DefaultPawnClass = nullptr;

	// Set default member values
	SecondPlayerActor = nullptr;
	MainMenuWidget = nullptr;
}

void AMainMenuGameMode::BeginPlay()
{
	auto* world = GetWorld();
	if (world)
	{
		// Set default number of players.
		auto* gameInstance = CastChecked<UUnrealSFASGameInstance>(UGameplayStatics::GetGameInstance(world));
		gameInstance->SetNumberOfPlayers(1);

		// Spawn weapons for the characters to hold without updating game UI as none has been created for main menu.
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(world, ASkeletalMeshActor::StaticClass(), actors);
		for (const auto& actor : actors)
		{
			if (actor->ActorHasTag(FName("SecondPlayerActor")))
			{
				SecondPlayerActor = actor;
				SecondPlayerActor->SetActorHiddenInGame(true);
				break;
			}
		}

		// Disable split screen
		world->GetGameViewport()->SetForceDisableSplitscreen(true);

		// Create a second potential player to recieve input from their gamepad
		auto* player2PC = CastChecked<AMainMenuPlayerController>(UGameplayStatics::CreatePlayer(world, 1));
		player2PC->SetPlayerIndex(1);

		// Create the main menu ui on the first player controller.
		auto* mainMenuPC = CastChecked<AMainMenuPlayerController>(UGameplayStatics::GetPlayerController(world, 0));
		MainMenuWidget = mainMenuPC->SpawnMainMenuUI();
	}
}
