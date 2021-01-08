// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup static mesh component
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	// Setup muzzle flash scene component
	MuzzleFlashScene = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleFlashScene"));
	MuzzleFlashScene->SetupAttachment(RootComponent);

	// Set default member values
	ShotAnimMontage = nullptr;
	ShotRecoverTime = 0.15f;
	ShotMaxRange = 1000.0f;
	MinDamage = 1;
	MaxDamage = 5;
	FireSound = nullptr;
	MuzzleFlashEmitterTemplate = nullptr;
}




