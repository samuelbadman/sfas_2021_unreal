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
	UFUNCTION(BlueprintImplementableEvent)
	void SetDotMaterial(UMaterialInstanceDynamic* Material);

	UFUNCTION(BlueprintImplementableEvent)
	void SetReticleVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent)
	void SetHitMarkerVisibility(bool Visible);

	UFUNCTION(BlueprintImplementableEvent)
	void SetWaveNumber(int wave);

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
