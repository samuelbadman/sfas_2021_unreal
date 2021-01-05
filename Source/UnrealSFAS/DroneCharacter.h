// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DroneCharacter.generated.h"

UCLASS()
class UNREALSFAS_API ADroneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADroneCharacter();

	/** Damages the drone character, reducing its hitpoints value. Returns whether the damage destroyed the drone character. Can be extended by derived blueprint */
	bool RecieveDamage(int Amount);

private:
	UPROPERTY(BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	int hitpoints;
};
