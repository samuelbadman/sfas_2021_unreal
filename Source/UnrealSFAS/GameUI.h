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

protected:
	void NativeConstruct() override;

private:
	UMaterialInstanceDynamic* DynamicDotMaterial;

	////////////////////////////////////
	/** UI category */
	/** Set in the derived blueprint */
	UPROPERTY(EditDefaultsOnly, Category = UI)
	UMaterialInterface* ParentDotMaterial;
	////////////////////////////////////
};
