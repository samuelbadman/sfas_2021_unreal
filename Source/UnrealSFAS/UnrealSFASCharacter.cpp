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
#include "UnrealSFASGameMode.h"

//////////////////////////////////////////////////////////////////////////
// AUnrealSFASCharacter

AUnrealSFASCharacter::AUnrealSFASCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	const float sensitivityScale = 0.95f;
	BaseTurnRate = 45.f * sensitivityScale;
	BaseLookUpRate = 45.f * sensitivityScale;

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

	// Enable custom depth stencil render on the player's mesh. This is used to mask the player in a post process material.
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(3);

	// Disable camera collision with the player.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

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
	MovingAccuracyDecreaseScale = 1.f;
	Aiming = false;
	AimingOverRightShoulder = true;
	HitMarkerDisplayDuration = 0.15f;
	Hitpoints = 100;
	MaximumHitpoints = Hitpoints;
	DefeatedTargetCameraOffset = FVector(0.f, 0.f, 100.f);
	DefeatedTargetCameraBoomLength = 300.f;
	Defeated = false;
	NumberOfEnemiesDefeated = 0;
	DamageDealt = 0;
	Accuracy = 0.f;
	Reloading = false;

	BulletImpactSound = nullptr;
	DefeatedSound = nullptr;
	HealingSound = nullptr;

	PlayerIndex = 0;

	CanReturnToMainMenu = false;
}

void AUnrealSFASCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Register AI stimuli source as a sight source
	AiStimuliSource->bAutoRegister = true;
	AiStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
}

void AUnrealSFASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraZoom(DeltaTime);
	UpdateCameraLocationOffset(DeltaTime);
	UpdateViewPitch(DeltaTime);

	// Calculate accuracy offset based on velocity. Standing still is 0, moving is up to max aim offset
	Accuracy = GetVelocity().Size() * MovingAccuracyDecreaseScale;

	// Update the reticle UI to reflect weapon accuracy.
	auto* world = GetWorld();
	if (world)
	{
		auto* unrealSFASController = Cast<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));
		if (unrealSFASController)
		{
			auto* gameUI = unrealSFASController->GetGameUI();
			if (gameUI)
			{
				gameUI->UpdateReticleTargetPosition(FMath::GetMappedRangeValueClamped(
					FVector2D(0.f, GetCharacterMovement()->MaxWalkSpeed * MovingAccuracyDecreaseScale),
					FVector2D(0.f, gameUI->GetMaxReticleSlateUnitOffset()),
					Accuracy));
				gameUI->InterpReticleToTargetPosition(DeltaTime);
			}
		}
	}

	// Force follow camera to look at the capsule component when the player has been defeated.
	if (Hitpoints <= 0)
	{
		FollowCamera->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), GetCapsuleComponent()->GetComponentLocation()));
	}
}

float AUnrealSFASCharacter::GetPitchOffset() const
{
	return UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Pitch;
}

void AUnrealSFASCharacter::SpawnWeapon(UGameUI* GameUI)
{
	// Is the default weapon class valid?
	if (DefaultWeaponClass)
	{
		// Is the world reference valid?
		auto* world = GetWorld();
		if (world)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			Weapon = world->SpawnActor<AWeapon>(DefaultWeaponClass.Get(), spawnParams);

			// Did the weapon spawn correctly?
			if (Weapon)
			{
				// Setup weapon
				Weapon->SetActorEnableCollision(false);
				Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);

				// Update game ui
				if (GameUI)
				{
					GameUI->SetWeaponRoundsRemaining(Weapon->GetRoundsRemaining());
					GameUI->SetWeaponClipSize(Weapon->GetClipCapacity());
				}
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

void AUnrealSFASCharacter::ReloadWeapon()
{
	// Check the character is not currently reloading.
	if (!Reloading)
	{
		// Check the weapon is valid.
		if (Weapon)
		{
			// Check the weapon clip is not currently full.
			if (!Weapon->IsClipFull())
			{
				// Check the weapon's reload montage is valid.
				auto* reloadMontage = Weapon->GetReloadAnimMontage();
				if (reloadMontage)
				{
					Reloading = true;

					PlayAnimMontage(reloadMontage);
				}
			}
		}
	}
}

void AUnrealSFASCharacter::PausePressed()
{
	auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(Controller);
	unrealSFASPlayerController->TogglePause();
}

void AUnrealSFASCharacter::PositiveMenuInputPressed()
{

}

void AUnrealSFASCharacter::NegativeMenuInputPressed()
{
	if (IsGamePaused())
	{
		ReturnToMainMenu();
	}

	if (Defeated && CanReturnToMainMenu)
	{
		ReturnToMainMenu();
	}
}

bool AUnrealSFASCharacter::IsGamePaused() const
{
	auto* unrealSFASPlayerController = Cast<AUnrealSFASPlayerController>(Controller);
	if (unrealSFASPlayerController)
	{
		return unrealSFASPlayerController->IsPaused();
	}

	return false;
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

		// Disable collision with the player capsule.
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Unregister the player from the AI sight sense.
		AiStimuliSource->UnregisterFromSense(UAISense_Sight::StaticClass());

		// Check the world is valid.
		auto* world = GetWorld();
		if (world)
		{
			// Play defeated audio.
			if (DefeatedSound)
			{
				UGameplayStatics::PlaySoundAtLocation(world, DefeatedSound, GetActorLocation());
			}

			// Cast to the player controller.
			auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));

			// Set input mode to game input only.
			unrealSFASPlayerController->SetInputMode(FInputModeGameOnly());

			// Hide the game UI.
			unrealSFASPlayerController->GetGameUI()->Show(false);

			// Show the game over UI.
			unrealSFASPlayerController->SpawnGameOverUI(NumberOfEnemiesDefeated, DamageDealt);

			// Notify the game mode a player has been defeated.
			auto* unrealSFASGameMode = CastChecked<AUnrealSFASGameMode>(UGameplayStatics::GetGameMode(world));
			unrealSFASGameMode->OnPlayerDefeated();
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
		auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));
		unrealSFASPlayerController->GetGameUI()->SetHitpointsValue(Hitpoints);
	}
}

void AUnrealSFASCharacter::CancelReload()
{
	if (Weapon)
	{
		auto* reloadMontage = Weapon->GetReloadAnimMontage();
		if (reloadMontage)
		{
			StopAnimMontage(reloadMontage);
		}
	}

	Reloading = false;
}

void AUnrealSFASCharacter::ReturnToMainMenu()
{
	// Return to the main menu
	const FName mainMenuLevelName = "MainMenu";
	auto* world = GetWorld();
	if (world)
	{
		// Remove the second player.
		UGameplayStatics::RemovePlayer(UGameplayStatics::GetPlayerController(GetWorld(), 1), true);

		// Open the main menu level.
		UGameplayStatics::OpenLevel(world, mainMenuLevelName);
	}
}

void AUnrealSFASCharacter::RecieveDamage(int Amount)
{
	// Check the world is valid.
	auto* world = GetWorld();
	if (world)
	{
		// Check if the incoming damage is negative.
		if (Amount < 0)
		{
			// Play healing sound.
			if (HealingSound)
			{
				UGameplayStatics::PlaySoundAtLocation(world, HealingSound, GetActorLocation());
			}
		}
		else
		{
			// Play impact sound.
			if (BulletImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(world, BulletImpactSound, GetActorLocation());
			}

			// Play impact UI animation.
			auto* unrealSFASPlayerController = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));
			auto* gameUI = unrealSFASPlayerController->GetGameUI();
			if (gameUI)
			{
				gameUI->PlayImpactAnim();
			}
		}
	}

	// Apply damage, ensuring hitpoints cannot exceed maximum.
	Hitpoints = FMath::Clamp(Hitpoints -= Amount, 0, MaximumHitpoints);

	// Check if the player has been defeated.
	if (Hitpoints == 0)
	{
		OnPlayerDefeated();
	}

	// Update the UI.
	UpdateHitpointsUI();
}

void AUnrealSFASCharacter::OnFinishedReload()
{
	if (Weapon)
	{
		// Insert a new clip into the weapon.
		Weapon->NewClip();

		// Update game UI
		auto* world = GetWorld();
		if (world)
		{
			auto* sfasPC = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));
			auto* gameUI = sfasPC->GetGameUI();
			if (gameUI)
			{
				gameUI->SetWeaponRoundsRemaining(Weapon->GetRoundsRemaining());

				// Hide reload prompt.
				gameUI->ShowReloadPrompt(false);
			}
		}
	}

	Reloading = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUnrealSFASCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AUnrealSFASCharacter::JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AUnrealSFASCharacter::AimWeapon);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AUnrealSFASCharacter::StopAimingWeapon);
	PlayerInputComponent->BindAction("FireWeapon", IE_Pressed, this, &AUnrealSFASCharacter::FireWeapon);
	PlayerInputComponent->BindAction("SwapShoulder", IE_Pressed, this, &AUnrealSFASCharacter::SwapShoulder);
	PlayerInputComponent->BindAction("ReloadWeapon", IE_Pressed, this, &AUnrealSFASCharacter::ReloadWeapon);

	// Bind the pause action method. Allow execution when the game is paused.
	FInputActionBinding& pauseBinding = PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AUnrealSFASCharacter::PausePressed);
	pauseBinding.bExecuteWhenPaused = true;

	// Bind the menu action methods. Allow execution when the game is paused.
	FInputActionBinding& PositiveMenuBinding = PlayerInputComponent->BindAction("PositiveMenu", IE_Pressed, this, &AUnrealSFASCharacter::PositiveMenuInputPressed);
	PositiveMenuBinding.bExecuteWhenPaused = true;

	FInputActionBinding& NegativeMenuBinding = PlayerInputComponent->BindAction("NegativeMenu", IE_Pressed, this, &AUnrealSFASCharacter::NegativeMenuInputPressed);
	NegativeMenuBinding.bExecuteWhenPaused = true;

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

void AUnrealSFASCharacter::SetPlayerIndex(int32 Index)
{
	PlayerIndex = Index;
}

void AUnrealSFASCharacter::SetupOnPossessed()
{
	// Store default view min and max pitches
	auto* world = GetWorld();
	if (world)
	{
		CameraManager = UGameplayStatics::GetPlayerCameraManager(world, PlayerIndex);
		if (CameraManager)
		{
			DefaultViewMinPitch = CameraManager->ViewPitchMin;
			DefaultViewMaxPitch = CameraManager->ViewPitchMax;
			TargetViewPitchMin = DefaultViewMinPitch;
			TargetViewPitchMax = DefaultViewMaxPitch;
		}
	}
}

void AUnrealSFASCharacter::SetCanReturnToMainMenu(bool CanReturn)
{
	CanReturnToMainMenu = CanReturn;
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

void AUnrealSFASCharacter::JumpPressed()
{
	// Cancel the reload if one is in progress.
	if (Reloading)
	{
		CancelReload();
	}

	Jump();
}

void AUnrealSFASCharacter::AimWeapon()
{
	if (!Defeated)
	{
		Aiming = true;
		TargetBoomLength = AimBoomLength;
		TargetCameraOffset = CameraAimOffset;
		AimBlendWeight = 1.f;
		TargetViewPitchMin = CameraAimMinPitch;
		TargetViewPitchMax = CameraAimMaxPitch;
		GetCharacterMovement()->MaxWalkSpeed = AimMaxWalkSpeed;
		bUseControllerRotationYaw = true;

		auto* world = GetWorld();
		if (world)
		{
			auto* unrealSFASController = Cast<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));
			if (unrealSFASController)
			{
				auto* gameUI = unrealSFASController->GetGameUI();
				if (gameUI)
				{
					gameUI->SetReticleVisibility(true);
				}
			}
		}
	}
}

void AUnrealSFASCharacter::StopAimingWeapon()
{
	if (!Defeated)
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

		auto* world = GetWorld();
		if (world)
		{
			auto* unrealSFASController = Cast<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));
			if (unrealSFASController)
			{
				auto* gameUI = unrealSFASController->GetGameUI();
				if (gameUI)
				{
					gameUI->SetReticleVisibility(false);
				}
			}
		}
	}
}

void AUnrealSFASCharacter::FireWeapon()
{
	// Is the player aiming and the weapon reference is valid?
	if (Aiming && !Reloading && (Weapon != nullptr))
	{
		// Is the world valid?
		auto* world = GetWorld();
		if (world)
		{
			// Check the weapon has rounds left in its clip.
			if (!Weapon->IsClipEmpty())
			{
				// Has enough game time elapsed since the last shot?
				const float currentGameSeconds = UKismetSystemLibrary::GetGameTimeInSeconds(world);
				if ((currentGameSeconds - GameSecondsAtLastShot) > Weapon->GetShotRecoverTime())
				{
					// Is the camera manager reference vallid?
					if (CameraManager)
					{
						GameSecondsAtLastShot = currentGameSeconds;

						// Play the weapon shot effects.
							// Play the weapon shot animation montage.
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

						CameraManager = UGameplayStatics::GetPlayerCameraManager(world, PlayerIndex);
						auto cameraLoc = CameraManager->GetCameraLocation();
						auto cameraForward = CameraManager->GetActorForwardVector();

						FHitResult hit;
						TArray<AActor*> ignoredActors;
						auto traceStart = cameraLoc;
						auto traceEnd = traceStart + (cameraForward * Weapon->GetShotMaxRange());

						// Calculate and apply deviationScale to the trace end point.
						auto deviationScale = FMath::GetMappedRangeValueClamped(
							FVector2D(0.f, GetCharacterMovement()->MaxWalkSpeed * MovingAccuracyDecreaseScale),
							FVector2D(0.f, Weapon->GetMaximumDeviation()),
							Accuracy
						);

						auto deviationDirection = UKismetMathLibrary::RotateAngleAxis(
							UKismetMathLibrary::Cross_VectorVector(cameraForward, FollowCamera->GetUpVector()),
							UKismetMathLibrary::RandomFloatInRange(0.f, 359.f),
							cameraForward);

						traceEnd += deviationDirection * deviationScale;

						// Trace in the ECC_Visibility channel for any actor except for self.
						if (UKismetSystemLibrary::LineTraceSingle(
							world,
							traceStart,
							traceEnd,
							UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
							false,
							ignoredActors,
							EDrawDebugTrace::None,
							hit,
							true))
						{
							// Check if the hit actor has the "Enemy" tag.
							auto* hitActor = hit.GetActor(); // Replaced hit.Actor from UE4
							if (hitActor->ActorHasTag(FName("Enemy")))
							{
								// Show the hit marker. Start a timer to hide the hitmarker.
								ShowHitMarker();
								GetWorldTimerManager().ClearTimer(HitMarkerTimerHandle);
								GetWorldTimerManager().SetTimer(HitMarkerTimerHandle, this, &AUnrealSFASCharacter::HideHitMarker, HitMarkerDisplayDuration, false);

								// Cast the hit actor to ADroneCharacter.
								auto* enemy = Cast<ADroneCharacter>(hitActor);

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

						// Remove a round from the current clip of the weapon.
						Weapon->RemoveRound();

						// Update the game ui.
						auto* sfasPC = CastChecked<AUnrealSFASPlayerController>(UGameplayStatics::GetPlayerController(world, PlayerIndex));
						auto* gameUI = sfasPC->GetGameUI();
						if (gameUI)
						{
							gameUI->SetWeaponRoundsRemaining(Weapon->GetRoundsRemaining());

							// Show reload prompt if weapon clip is empty.
							if (Weapon->IsClipEmpty())
							{
								// Auto reload the weapon when it is empty.
								ReloadWeapon();
							}
						}
					}
				}
			}
			else
			{
				// Play empty clip sound.
				auto* emptySound = Weapon->GetEmptyFireSound();
				if (emptySound)
				{
					UGameplayStatics::PlaySoundAtLocation(world, emptySound, GetActorLocation());
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
