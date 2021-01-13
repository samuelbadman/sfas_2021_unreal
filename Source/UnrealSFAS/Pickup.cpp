// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "UnrealSFASCharacter.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Setup static mesh component
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = Mesh;
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Setup sphere collision component
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->InitSphereRadius(32.f);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnBeginOverlap);

	// Setup rotating movement component
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.f, 5.f, 0.f);
	RotatingMovement->bRotationInLocalSpace = true;

	// Set member default values.
	AliveDuration = 30.f;
}

void APickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the other actor is the player.
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		// Cast to UnrealSFASCharacter.
		auto* unrealSFASCharacter = CastChecked<AUnrealSFASCharacter>(OtherActor);

		OnPickedUpByPlayer(unrealSFASCharacter);
	}
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	// Set a timer that destroys the pickup after its alive duration has expired.
	GetWorldTimerManager().SetTimer(AliveTimerHandle, this, &APickup::OnAliveDurationExpired, AliveDuration, false);
}

void APickup::OnPickedUpByPlayer(AUnrealSFASCharacter* Player)
{
	// Stop the alive timer. The pickup actor state should be managed by the OnBeginOverlap override.
	GetWorldTimerManager().ClearTimer(AliveTimerHandle);
}

void APickup::OnAliveDurationExpired()
{
	// Destroy the actor.
	this->Destroy();
}
