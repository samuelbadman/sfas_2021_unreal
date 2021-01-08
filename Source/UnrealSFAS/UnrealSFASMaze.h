// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealSFASMaze.generated.h"

UCLASS()
class UNREALSFAS_API AUnrealSFASMaze : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnrealSFASMaze();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, Category = Maze)
	UStaticMesh* WallMesh;

	UPROPERTY(EditDefaultsOnly, Category = Maze)
	UMaterialInterface* WallMaterial;
};
