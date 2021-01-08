// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealSFASCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionSystem.h"
#include "UnrealSFASPlayerController.h"
#include "GameUI.h"
#include "DroneCharacter.h"
#include "Pause/PauseUserWidget.h"

//////////////////////////////////////////////////////////////////////////
// AUnrealSFASCharacter

AUnrealSFASCharacter::AUnrealSFASCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 200.f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 175.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->TargetOffset.Z = 65.0f;

	// Initialise TargetBoomLength to CameraBoom->TargetArmLength's default length
	DefaultBoomLength = CameraBoom->TargetArmLength;
	TargetBoomLength = DefaultBoomLength;

	// Initialise TargetCameraOffset to CameraBoom's socket offset
	TargetCameraOffset = CameraBoom->SocketOffset;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetRelativeLocation(FVector(0.f, 35.f, 0.f));
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	DefaultCameraRelativeLocation = FollowCamera->GetRelativeLocation();

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Create AI stimuli source component
	AiStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("SightStimuliSource"));

	// Add "Player" tag to actor
	Tags.Add(FName("Player"));

	// Set collision response to visibility channel to block.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// Set default member values
	CameraZoomSpeed = 4.f;
	CameraMoveSpeed = 4.f;
	ViewPitchAdjustSpeed = 4.f;
	AimBlendWeight = 0.f;
	AimBoomLength = 150.f;
	CameraAimOffset = FVector(0.f, 50.f, 50.f);
	CameraAimMinPitch = -35.f;
	CameraAimMaxPitch = 35.f;
	TargetViewPitchMin = 0.f;
	TargetViewPitchMax = 0.f;
	GameSecondsAtLastShot = 0.f;
	AimMaxWalkSpeed = 275.f;
	Aiming = false;
	AimingOverRightShoulder = true;
	HitMarkerDisplayDuration = 0.15f;
	Hitpoints = 100;
	DefeatedTargetCameraOffset = FVector(0.f, 0.f, 100.f);
	DefeatedTargetCameraBoomLength = 300.f;
	Defeated = false;
	NumberOfEnemiesDefeated = 0;
	DamageDealt = 0;

	BulletImpactSound = nullptr;
	DefeatedSound = nullptr;
}

void AUnrealSFASCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Register AI stimuli source as a sight source
	AiStimuliSource->bAutoRegister = true;
	AiStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());

	// Store default view min and max pitches
	auto* world = GetWorld();
	if (world)
	{
		CameraManager = UGameplayStatics::GetPlayerCameraManager(world, 0);
		if (CameraManager)
		{
			DefaultViewMinPitch = CameraManager->ViewPitchMin;
			DefaultViewMaxPitch = CameraManager->ViewPitchMax;
			TargetViewPitchMin = DefaultViewMinPitch;
			TargetViewPitchMax = DefaultViewMaxPitch;
		}
	}

	SpawnWeapon();
}

void AUnrealSFASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraZoom(DeltaTime);
	UpdateCameraLocationOffset(DeltaTime);
	UpdateViewPitch(DeltaTime);

	// Force follow camera to look at the capsule component.
	if (Hitpoints <= 0)
	{
		FollowCamera->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), GetCapsuleComponent()->GetComponentLocation()));
	}
}

float AUnrealSFASCharacter::GetPitchOffset() const
{
	return UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Pitch;
}

void AUnrealSFASCharacter::SpawnWeapon()
{
	// Is the default weapon class valid?
	if (DefaultWeaponClass)
	{
		// Is the world reference valid?
		auto* world = GetWorld();
		if (world)
		{
			Weapon = world->SpawnActor<AWeapon>(DefaultWeaponClass.Get());

			// Did the weapon spawn correctly?
			if (Weapon)
			{
				// Setup weapon
				Weapon->SetActorEnableCollision(false);
				Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
			}
		}
	}
}

void AUnrealSFASCharacter::UpdateCameraZoom(const float DeltaTime)
{
	// If CameraBoom's target arm length is not nearly equal to target camera boom length
	if (!FMath::IsNearlyEqual(CameraBoom->TargetArmLength, TargetBoomLength, 1.f))
	{
		// Smoothly interpolate CameraBoom's target arm length towards the target camera boom length
		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetBoomLength, DeltaTime, CameraZoomSpeed);
	}
}

void AUnrealSFASCharacter::UpdateCameraLocationOffset(const float DeltaTime)
{
	// If CameraBoom's socket offset does not equal the target camera offset
	if (CameraBoom->SocketOffset != TargetCameraOffset)
	{
		// Smoothly interpolate CameraBoom's socket offset to the target camera offset
		CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetCameraOffset, DeltaTime, CameraMoveSpeed);
	}
}

void AUnrealSFASCharacter::UpdateViewPitch(const float DeltaTime)
{
	if (CameraManager)
	{
		if (!FMath::IsNearlyEqual(CameraManager->ViewPitchMin, TargetViewPitchMin, 1.f))
		{
			CameraManager->ViewPitchMin = FMath::FInterpTo(CameraManager->ViewPitchMin, TargetViewPitchMin, DeltaTime, ViewPitchAdjustSpeed);
		}

		if (!FMath::IsNearlyEqual(CameraManager->ViewPitchMax, TargetViewPitchMax, 1.f))
		{
			CameraManager->ViewPitchMax = FMath::FInterpTo(CameraManager->ViewPitchMax, TargetViewPitchMax, DeltaTime, ViewPitchAdjustSpeed);
		}
	}
}

void AUnrealSFASCharacter::SwapAimingShoulder()
{
	if (AimingOverRightShoulder)
	{
		// Swap to over left shoulder
		TargetCameraOffset.Y = -CameraAimOffset.Y;
		FVector camRelLoc = FollowCamera->GetRelativeLocation();
		FollowCamera->SetRelativeLocation(FVector(camRelLoc.X, -camRelLoc.Y, camRelLoc.Z));
		GetMesh()->SetRelativeScale3D(FVector(-1.f, 1.f, 1.f));
		AimingOverRightShoulder = false;
		// Also swap the attached weapon's scale
		if (Weapon)
		{
			Weapon->SetActorRelativeScale3D(FVector(-1.f, 1.f, 1.f));
		}
	}
	else
	{
		// Swap to over right shoulder
		TargetCameraOffset.Y = CameraAimOffset.Y;
		GetMesh()->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
		FollowCamera->SetRelativeLocation(DefaultCameraRelativeLocation);
		AimingOverRightShoulder = true;
		// Also swap the attached weapon's scale
		if (Weapon)
		{
			Weapon->SetActorRelativeScale3D(FVector(1.f, 1.f, 1.f));
		}
	}
}

void AUnrealSFASCharacter::PausePressed()
{
	auto* UnrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(Controller);
	UnrealSFASPlayerController->TogglePause();
}

void AUnrealSFASCharacter::ShowHitMarker()
{
	auto* UnrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(Controller);
	UnrealSFASPlayerController->GetGameUI()->SetHitMarkerVisibility(true);
}

void AUnrealSFASCharacter::HideHitMarker()
{
	auto* UnrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(Controller);
	UnrealSFASPlayerController->GetGameUI()->SetHitMarkerVisibility(false);
}

void AUnrealSFASCharacter::OnPlayerDefeated()
{
	// Check if the player has already been defeated
	if (!Defeated)
	{
		// Set defeated flag.
		Defeated = true;

		// Ragdoll skeleton.
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		// Set target camera offsets to be above and away from the player.
		TargetCameraOffset = DefeatedTargetCameraOffset;
		TargetBoomLength = DefeatedTargetCameraBoomLength;

		// Stop the character moving.
		GetCharacterMovement()->MaxWalkSpeed = 0.f;

		// Check the world is valid.
		auto* world = GetWorld();
		if (world)
		{
			// Play defeated audio.
			if (DefeatedSound)
			{
				UGameplayStatics::PlaySoundAtLocation(world, DefeatedSound, GetActorLocation());
			}

			// Check player controller is valid.
			auto* playerController = UGameplayStatics::GetPlayerController(world, 0);
			if (playerController)
			{
				// Cast to the player controller.
				auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(Controller);

				// Set input mode to UI input only.
				playerController->SetInputMode(FInputModeUIOnly());

				// Show the mouse cursor.
				playerController->bShowMouseCursor = true;

				// Check the player controller cast correctly.
				if (unrealSFASPlayerController)
				{
					// Hide the game UI.
					unrealSFASPlayerController->GetGameUI()->Show(false);

					// Show the game over UI.
					unrealSFASPlayerController->SpawnGameOverUI(NumberOfEnemiesDefeated, DamageDealt);
				}
			}
		}
	}
}

void AUnrealSFASCharacter::UpdateHitpointsUI()
{
	// Check the world is valid.
	auto* world = GetWorld();
	if (world)
	{
		// Set the new HP value.
		auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, 0));
		unrealSFASPlayerController->GetGameUI()->SetHitpointsValue(Hitpoints);
	}
}

void AUnrealSFASCharacter::RecieveDamage(int Amount)
{
	// Negative damage will heal the player.

	// Play impact sound.
	if (BulletImpactSound)
	{
		auto* world = GetWorld();
		if (world)
		{
			UGameplayStatics::PlaySoundAtLocation(world, BulletImpactSound, GetActorLocation());
		}
	}

	// Apply damage.
	Hitpoints -= Amount;

	// Check if the player has been defeated.
	if (Hitpoints <= 0)
	{
		OnPlayerDefeated();
	}

	// Update the UI.
	UpdateHitpointsUI();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUnrealSFASCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AUnrealSFASCharacter::AimWeapon);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AUnrealSFASCharacter::StopAimingWeapon);
	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AUnrealSFASCharacter::FireWeapon);
	PlayerInputComponent->BindAction("SwapShoulder", IE_Pressed, this, &AUnrealSFASCharacter::SwapShoulder);

	// Bind the pause action method. Allow execution when the game is paused.
	FInputActionBinding& pauseBinding = PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AUnrealSFASCharacter::PausePressed);
	pauseBinding.bExecuteWhenPaused = true;

	PlayerInputComponent->BindAxis("MoveForward", this, &AUnrealSFASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUnrealSFASCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AUnrealSFASCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AUnrealSFASCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AUnrealSFASCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AUnrealSFASCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AUnrealSFASCharacter::OnResetVR);
}

void AUnrealSFASCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AUnrealSFASCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AUnrealSFASCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AUnrealSFASCharacter::AimWeapon()
{
	Aiming = true;
	TargetBoomLength = AimBoomLength;
	TargetCameraOffset = CameraAimOffset;
	AimBlendWeight = 1.f;
	TargetViewPitchMin = CameraAimMinPitch;
	TargetViewPitchMax = CameraAimMaxPitch;
	GetCharacterMovement()->MaxWalkSpeed = AimMaxWalkSpeed;
	bUseControllerRotationYaw = true;

	auto* UnrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(Controller);
	UnrealSFASPlayerController->GetGameUI()->SetReticleVisibility(true);
}

void AUnrealSFASCharacter::StopAimingWeapon()
{
	Aiming = false;
	TargetBoomLength = DefaultBoomLength;
	TargetCameraOffset = FVector::ZeroVector;
	AimBlendWeight = 0.f;
	TargetViewPitchMin = DefaultViewMinPitch;
	TargetViewPitchMax = DefaultViewMaxPitch;
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	bUseControllerRotationYaw = false;
	if (!AimingOverRightShoulder)
	{
		SwapAimingShoulder();
	}

	auto* UnrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(Controller);
	UnrealSFASPlayerController->GetGameUI()->SetReticleVisibility(false);
}

void AUnrealSFASCharacter::FireWeapon()
{
	// Is the player aiming and the weapon reference is valid?
	if (Aiming && (Weapon != nullptr))
	{
		// Is the world valid?
		auto* world = GetWorld();
		if (world)
		{
			// Has enough game time elapsed since the last shot?
			const float currentGameSeconds = UKismetSystemLibrary::GetGameTimeInSeconds(world);
			if ((currentGameSeconds - GameSecondsAtLastShot) > Weapon->GetShotRecoverTime())
			{
				// Is the camera manager reference vallid?
				if (CameraManager)
				{
					GameSecondsAtLastShot = currentGameSeconds;

					auto cameraLoc = CameraManager->GetCameraLocation();
					auto cameraForward = CameraManager->GetActorForwardVector();

					// Play the weapon shot animation monatage.
					if (Weapon)
					{
						auto* montage = Weapon->GetShotAnimMontage();
						if (montage)
						{
							PlayAnimMontage(montage);
						}

						// Play the weapon fire sound at the player's location.
						auto* sound = Weapon->GetFireSound();
						if (sound)
						{
							UGameplayStatics::PlaySoundAtLocation(world, sound, GetActorLocation());
						}

						// Spawn muzzle flash particles system.
						auto* muzzleEmitterTemplate = Weapon->GetMuzzleFlashEmitterTemplate();
						if (muzzleEmitterTemplate)
						{
							auto* muzzleScene = Weapon->GetMuzzeFlashScene();
							UGameplayStatics::SpawnEmitterAttached(muzzleEmitterTemplate, muzzleScene, FName("None"), muzzleScene->GetComponentLocation(),
								muzzleScene->GetComponentRotation(), EAttachLocation::KeepWorldPosition, true, EPSCPoolMethod::AutoRelease);
						}
					}

					FHitResult hit;
					TArray<AActor*> ignoredActors;
					// Trace in the ECC_Visibility channel for any actor except for self.
					if (UKismetSystemLibrary::LineTraceSingle(
						world, 
						cameraLoc,
						cameraLoc + (cameraForward * Weapon->GetShotMaxRange()), 
						UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), 
						false,
						ignoredActors, 
						EDrawDebugTrace::None, 
						hit,
						true))
					{
						// Check if the hit actor has the "Enemy" tag.
						if (hit.Actor->ActorHasTag(FName("Enemy")))
						{
							// Show the hit marker. Start a timer to hide the hitmarker.
							ShowHitMarker();
							GetWorldTimerManager().ClearTimer(HitMarkerTimerHandle);
							GetWorldTimerManager().SetTimer(HitMarkerTimerHandle, this, &AUnrealSFASCharacter::HideHitMarker, HitMarkerDisplayDuration, false);

							// Cast the hit actor to ADroneCharacter.
							auto* enemy = Cast<ADroneCharacter>(hit.Actor);

							// Check the cast was successful.
							if (enemy)
							{
								// Damage the enemy.
								int32 damage = FMath::RandRange(Weapon->GetMinDamage(), Weapon->GetMaxDamage());
								if (enemy->RecieveDamage(damage))
								{
									NumberOfEnemiesDefeated++;
								}
								DamageDealt += damage;
							}
						}
					}
				}
			}
		}
	}
}

void AUnrealSFASCharacter::SwapShoulder()
{
	if (Aiming)
	{
		SwapAimingShoulder();
	}
}

void AUnrealSFASCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealSFASCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AUnrealSFASCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AUnrealSFASCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
