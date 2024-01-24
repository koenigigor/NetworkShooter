// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "EquipmentAttributes.generated.h"

USTRUCT()
struct FAttributeApply
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float Value = 1;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EGameplayModOp::Type> ApplyMethod = EGameplayModOp::Additive;
};


/**
 * 
 */
UCLASS(HideCategories=("Period", "Application", "Overflow", "Display", "Tags", "Immunity", "Stacking", "Granted Abilities", "Expiration", "GameplayEffect"))
class NETWORKSHOTER_API UEquipmentAttributes : public UGameplayEffect
{
	GENERATED_BODY()

	UEquipmentAttributes();

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayAttribute, FAttributeApply> AttributesToApply;
	
	void OverrideModifiers();

	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
};
