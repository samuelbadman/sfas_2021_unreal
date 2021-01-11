// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSFASMaze.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AUnrealSFASMaze::AUnrealSFASMaze()
{
 	// No need to tick the maze at this point
	PrimaryActorTick.bCanEverTick = false;

	// Set actor defaults
	WallMesh = nullptr;
	WallMaterial = nullptr;
}

// Called when the game starts or when spawned
void AUnrealSFASMaze::BeginPlay()
{
	Super::BeginPlay();

	if (WallMesh)
	{
		const int32 mazeSize = 20;
		const float blockSize = 200.0f;
		const float blockWidth = 2.0f;
		const float mazeDensity = 0.1f; // In the range 0 - 1. 1 is more dense
		const float tallBlockZPos = 150.0f;
		const float tallBlockHeight = 3.f;
		const float shortBlockZPos = 75.0f;
		const float shortBlockHeight = 1.5f;
		const float tallBlockDensity = 0.6f; // In the range 0 - 1. 1 is more dense

		if (mazeSize > 0)
		{
			// Array of binary values: 1 = wall, 0 = space
			uint32 mazeArray[mazeSize];
			const uint32 numBits = (sizeof(mazeArray) / mazeSize) * CHAR_BIT;

			for (size_t i = 0; i < mazeSize; i++)
			{
				uint32 value = 0;

				// Set random bits of value
				for (uint32 n = 0; n < numBits; n++)
				{
					if (UKismetMathLibrary::RandomBoolWithWeight(mazeDensity))
					{
						value |= (1 << n);
					}
				}

				// Add the value to the maze
				mazeArray[i] = value;
			}

			float xPos = 0.0f;
			float yPos = 0.0f;
			FQuat worldRotation(FVector(0.0f, 0.0f, 1.0f), 0.0f);
			FVector worldScale(blockWidth, blockWidth, 0.0f);
			uint32 mazeRow;

			USceneComponent* rootComponent = GetRootComponent();

			// Loop through the binary values to generate the maze as static mesh components attached to the root of this actor
			for (int32 i = 0; i < mazeSize; i++)
			{
				yPos = static_cast<float>(i - (mazeSize / 2)) * blockSize;
				mazeRow = mazeArray[i];

				for (int32 j = 0; j < mazeSize; j++)
				{
					xPos = static_cast<float>(j - (mazeSize / 2)) * blockSize;

					uint32 mazeValue = (mazeRow >> (mazeSize - (j + 1))) & 1;

					if (mazeValue)
					{
						float blockZPos = shortBlockZPos;
						float blockHeight = shortBlockHeight;

						if (UKismetMathLibrary::RandomBoolWithWeight(tallBlockDensity))
						{
							blockZPos = tallBlockZPos;
							blockHeight = tallBlockHeight;
						}

						worldScale.Z = blockHeight;

						UStaticMeshComponent* meshComponent = NewObject<UStaticMeshComponent>(this);
						FVector worldPosition(xPos, yPos, blockZPos);
						FTransform worldXForm(worldRotation, worldPosition, worldScale);

						meshComponent->SetStaticMesh(WallMesh);
						meshComponent->SetWorldTransform(worldXForm);
						meshComponent->AttachToComponent(rootComponent, FAttachmentTransformRules::KeepWorldTransform);

						// Used for post process outline effect.
						meshComponent->SetRenderCustomDepth(true);
						meshComponent->SetCustomDepthStencilValue(2);

						// Set the wall material if valid
						if (WallMaterial)
						{
							meshComponent->SetMaterial(0, WallMaterial);
						}


						// Set the mesh component to affect the nav mesh
						meshComponent->SetCanEverAffectNavigation(true);

						meshComponent->RegisterComponent();
					}
				}
			}
		}
	}
}

