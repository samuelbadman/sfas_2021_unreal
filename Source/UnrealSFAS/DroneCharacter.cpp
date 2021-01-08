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

	// Set spawn collision handling to adjust if possible but always spawn the actor.
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Setup muzzle flash scene.
	MuzzleFlashScene = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleFlashScene"));
	MuzzleFlashScene->SetupAttachment(GetMesh());

	// Set member default values.
	hitpoints = 100;
	BulletImpactSound = nullptr;
	DestroyedSound = nullptr;
	MuzzleFlashEmitterTemplate = nullptr;
	ExplosionEmitterTemplate = nullptr;

	// The enemy drone AI controller will add the "Enemy" tag when this character is possessed by it.
}

bool ADroneCharacter::RecieveDamage(int Amount)
{
	// Recieving negative damage can heal the drone.
	//if (Amount <= 0.f) return false;

	auto* world = GetWorld();
	if (world)
	{
		// Play the impact sound.
		if (BulletImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(world, BulletImpactSound, GetActorLocation());
		}

		// Apply the damage.
		hitpoints -= Amount;

		// Check if the drone's hitpoints have been reduced to 0.
		if (hitpoints <= 0)
		{
			// Notify the game mode a drone has beeen destroyed.
			auto* unrealSFASGameMode = CastChecked<AUnrealSFASGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			unrealSFASGameMode->NotifyDroneDestroyed();

			// Play the destroyed sound.
			if (DestroyedSound)
			{
				UGameplayStatics::PlaySoundAtLocation(world, DestroyedSound, GetActorLocation());
			}

			// Spawn explosion particle system.
			if (ExplosionEmitterTemplate)
			{
				auto* mesh = GetMesh();
				if (mesh)
				{
					UGameplayStatics::SpawnEmitterAtLocation(world, ExplosionEmitterTemplate, mesh->GetComponentLocation(), mesh->GetComponentRotation(), true, EPSCPoolMethod::AutoRelease);
				}
			}

			// Destroy the drone actor.
			this->Destroy();

			return true;
		}
	}

	return false;
}
