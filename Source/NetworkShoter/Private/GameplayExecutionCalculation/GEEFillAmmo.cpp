// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayExecutionCalculation/GEEFillAmmo.h"
#include "WeaponAttributeSet.h"
#include "AbilitySystemComponent.h"

struct FAttribCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Ammo);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxAmmo);
	
	FAttribCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWeaponAttributeSet, Ammo, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWeaponAttributeSet, MaxAmmo, Source, false);
	}
};



static const FAttribCapture& GetAttributeCapture()
{
	static FAttribCapture Capture;
	return Capture;
}

UGEEFillAmmo::UGEEFillAmmo()
{
	RelevantAttributesToCapture.Add(GetAttributeCapture().AmmoDef);
	RelevantAttributesToCapture.Add(GetAttributeCapture().MaxAmmoDef);
}

void UGEEFillAmmo::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	
	float MaxAmmo = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(GetAttributeCapture().MaxAmmoProperty);
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture().MaxAmmoDef,  FAggregatorEvaluateParameters(), MaxAmmo);
	
	float CurrentAmmo = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(GetAttributeCapture().AmmoProperty);
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture().AmmoDef,  FAggregatorEvaluateParameters(), CurrentAmmo);
	
	
	float AmmoToAdd = MaxAmmo - CurrentAmmo;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture().AmmoProperty, EGameplayModOp::Additive, AmmoToAdd));
}
