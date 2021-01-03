// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class UNREALSFAS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Components)
	FORCEINLINE UStaticMeshComponent* GetStaticMesh() const { return StaticMesh; }

	FORCEINLINE class UAnimMontage* GetShotAnimMontage() const { return ShotAnimMontage; }
	FORCEINLINE float GetShotRecoverTime() const { return ShotRecoverTime; }
	FORCEINLINE float GetShotMaxRange() const { return ShotMaxRange; }
	FORCEINLINE int GetMinDamage() const { return MinDamage; }
	FORCEINLINE int GetMaxDamage() const { return MaxDamage; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	/** Weapon category */
	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	class UAnimMontage* ShotAnimMontage;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float ShotRecoverTime;

	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	float ShotMaxRange;

	/** Set in the derived blueprint. The minimum amount of damage a shot of this weapon can do. Used as the bottom of a range. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	int MinDamage;

	/** Set in the derived blueprint. The maximum amount of damage a shot of this weapon can do. Used as the top of a range. */
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	int MaxDamage;
	/////////////////////////////////////////////////////////////
};
