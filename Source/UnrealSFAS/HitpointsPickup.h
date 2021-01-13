// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HitpointsPickup.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API AHitpointsPickup : public APickup
{
	GENERATED_BODY()
	
public:
	AHitpointsPickup();

	/** Override of APickup's OnPickedUpByPlayer method. */
	void OnPickedUpByPlayer(class AUnrealSFASCharacter* Player) override;

private:
	/** The amount of hitpoints to add to the player's hitpoints total on pickup. The player's total hitpoints will be clamped to their maximum hitpoints value if the maximum is exceeded. */
	UPROPERTY(EditDefaultsOnly,Category = Pickup, Meta = (AllowPrivateAccess = "true"))
	int Amount;
};
