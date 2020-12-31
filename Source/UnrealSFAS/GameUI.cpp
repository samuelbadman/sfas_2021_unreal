// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI.h"

void UGameUI::NativeConstruct()
{
	Super::NativeConstruct();

	// Check the parent dot material is valid
	if (ParentDotMaterial)
	{
		DynamicDotMaterial = UMaterialInstanceDynamic::Create(ParentDotMaterial, this);

		// Check the dynamic material created succesfully
		if (DynamicDotMaterial)
		{
			SetDotMaterial(DynamicDotMaterial);
		}
	}
}
