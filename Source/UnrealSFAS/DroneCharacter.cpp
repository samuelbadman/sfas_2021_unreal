// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneCharacter.h"

// Sets default values
ADroneCharacter::ADroneCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set the skeletal mesh component to block the visibility collision channel.
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// Set the AI controller to possess the pawn when placed in the world or spawned.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


