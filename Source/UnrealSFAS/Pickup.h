// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class UNREALSFAS_API APickup : public AActor
{
	GENERATED_BODY()

	/** Static mesh representation of the pickup in the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	/** The collision component used to detect the player. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Sphere;

	/** Rotates the actor on its origin. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pickup, meta = (AllowPrivateAccess = "true"))
	class URotatingMovementComponent* RotatingMovement;

public:	
	// Sets default values for this actor's properties
	APickup();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called when the player overlaps the sphere component. */
	virtual void OnPickedUpByPlayer(class AUnrealSFASCharacter* Player);

private:
	UFUNCTION()
	void OnAliveDurationExpired();
	FTimerHandle AliveTimerHandle;

	/** The duration in seconds to remain alive for once dropped. Upon expiration the pickup is destroyed. */
	UPROPERTY(EditDefaultsOnly, Category = Pickup, Meta = (AllowPrivateAccess = "true"))
	float AliveDuration;
};
