// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameUI.generated.h"

/**
 * 
 */
UCLASS()
class UNREALSFAS_API UGameUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void SetDotMaterial(UMaterialInstanceDynamic* Material);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void SetReticleVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void SetHitMarkerVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void SetWaveNumber(int wave);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void Show(bool Show);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void ShowWaveStatusNotification(int CurrentWaveNumber, bool WaveComplete);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void HideWaveStatusNotification();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void SetHitpointsValue(int Hitpoints);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reticle")
	void UpdateReticleTargetPosition(float Accuracy);

	UFUNCTION(BlueprintImplementableEvent, Category = "Reticle")
	void InterpReticleToTargetPosition(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void SetWeaponRoundsRemaining(int RoundsRemaining);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void SetWeaponClipSize(int ClipSize);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void ShowReloadPrompt(bool Show);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game UI")
	void PlayImpactAnim();

	FORCEINLINE float GetMaxReticleSlateUnitOffset() const { return MaxReticleSlateUnitOffset; }

protected:
	void NativeConstruct() override;

private:
	UMaterialInstanceDynamic* DynamicDotMaterial;

	UPROPERTY(BlueprintReadWrite, Category = "Reticle", meta = (AllowPrivateAccess = "true"))
	FVector2D ReticleNorthTargetPosition;
	UPROPERTY(BlueprintReadWrite, Category = "Reticle", meta = (AllowPrivateAccess = "true"))
	FVector2D ReticleSouthTargetPosition;
	UPROPERTY(BlueprintReadWrite, Category = "Reticle", meta = (AllowPrivateAccess = "true"))
	FVector2D ReticleEastTargetPosition;
	UPROPERTY(BlueprintReadWrite, Category = "Reticle", meta = (AllowPrivateAccess = "true"))
	FVector2D ReticleWestTargetPosition;

	////////////////////////////////////
	/** UI category */
	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UMaterialInterface* ParentDotMaterial;

	/** Maximum position offset of each part of the reticle when accuracy decreases. */
	UPROPERTY(EditDefaultsOnly, Category = Reticle)
	float MaxReticleSlateUnitOffset;

	/** Speed to interpolate reticle positions when adjusting for accuracy. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Reticle, meta = (AllowPrivateAccess = "true"))
	float ReticleInterpolationSpeed;
	////////////////////////////////////
};
