// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class UNREALSFAS_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Volume, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Volume;

public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	/** Gets the volume box component. */
	FORCEINLINE class UBoxComponent* GetVolume() const { return Volume; }
};
