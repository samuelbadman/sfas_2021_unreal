// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneCharacter.h"
#include "UnrealSFASGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HitpointsPickup.h"

// Sets default values
ADroneCharacter::ADroneCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the skeletal mesh component to block the visibility collision channel.
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// Set the AI controller to possess the pawn when placed in the world or spawned.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Set spawn collision handling to adjust if possible but always spawn the actor.
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Setup muzzle flash scene.
	MuzzleFlashScene = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleFlashScene"));
	MuzzleFlashScene->SetupAttachment(GetMesh());

	// Setup motor audio source audio component.
	MotorAudioSource = CreateDefaultSubobject<UAudioComponent>(TEXT("MotorAudioSource"));
	MotorAudioSource->SetupAttachment(GetMesh());
	MotorAudioSource->SetVolumeMultiplier(0.75f);
	DefaultMotorAudioPitchMultiplier = 0.125f;
	MotorAudioSource->SetPitchMultiplier(DefaultMotorAudioPitchMultiplier);

	// Set member default values.
	Hitpoints = 100;
	MaxMotorAudioPitchMultiplierModifier = 4.f;
	BulletImpactSound = nullptr;
	DestroyedSound = nullptr;
	MuzzleFlashEmitterTemplate = nullptr;
	ExplosionEmitterTemplate = nullptr;
	PickupDropRate = 0.5f;
	PickupClassToDrop = nullptr;

	// The enemy drone AI controller will add the "Enemy" tag when this character is possessed by it.
}

void ADroneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Adjust the motor audio pitch based on the character's velocity. Higher velocity = higher pitch multiplier
	auto mapped = UKismetMathLibrary::MapRangeClamped(GetVelocity().Size(), 0.f, GetCharacterMovement()->MaxWalkSpeed, 0.f, 4.f);
	MotorAudioSource->SetPitchMultiplier(FMath::Clamp(DefaultMotorAudioPitchMultiplier + mapped, DefaultMotorAudioPitchMultiplier, MaxMotorAudioPitchMultiplierModifier));
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
		Hitpoints -= Amount;

		// Check if the drone's Hitpoints have been reduced to 0.
		if (Hitpoints <= 0)
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

			// Should the drone drop a pickup?
			if (UKismetMathLibrary::RandomBoolWithWeight(PickupDropRate))
			{
				// Spawn the set pickup actor.
				if (PickupClassToDrop)
				{
					world->SpawnActor<AHitpointsPickup>(PickupClassToDrop.Get(), GetActorLocation(), GetActorRotation());
				}
			}

			// Destroy the drone actor.
			this->Destroy();

			return true;
		}
	}

	return false;
}

void ADroneCharacter::AddHitpoints(int Amount)
{
	Hitpoints += Amount;
}
