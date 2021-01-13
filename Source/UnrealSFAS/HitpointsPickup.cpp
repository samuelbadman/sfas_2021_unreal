// Fill out your copyright notice in the Description page of Project Settings.


#include "HitpointsPickup.h"
#include "UnrealSFASCharacter.h"

AHitpointsPickup::AHitpointsPickup()
{
	// Set default member values.
	Amount = 10;
}

void AHitpointsPickup::OnPickedUpByPlayer(AUnrealSFASCharacter* Player)
{
	Super::OnPickedUpByPlayer(Player);

	// Check Player is valid.
	if (Player)
	{
		// Deal negative damage to the player, which will heal them.
		Player->RecieveDamage(-Amount);

		// Destroy the pickup actor.
		this->Destroy();
	}
}
