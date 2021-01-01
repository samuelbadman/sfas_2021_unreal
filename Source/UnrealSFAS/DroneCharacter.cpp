// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneCharacter.h"
#include "UnrealSFASGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADroneCharacter::ADroneCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set the skeletal mesh component to block the visibility collision channel.
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// Set the AI controller to possess the pawn when placed in the world or spawned.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Set member default values.
	hitpoints = 100.f;
}

bool ADroneCharacter::RecieveDamage(float Amount)
{
	// Recieving negative damage can heal the drone.
	//if (Amount <= 0.f) return false;

	// Apply the damage.
	hitpoints -= Amount;

	// Check if the drone's hitpoints have been reduced to 0.
	if (hitpoints <= 0.f)
	{
		// Notify the game mode a drone has beeen destroyed.
		auto* unrealSFASGameMode = CastChecked<AUnrealSFASGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		unrealSFASGameMode->NotifyDroneDestroyed();

		// Destroy the drone actor.
		this->Destroy();

		return true;
	}

	return false;
}
