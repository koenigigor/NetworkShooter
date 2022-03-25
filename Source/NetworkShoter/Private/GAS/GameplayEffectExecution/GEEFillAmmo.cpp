// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectExecution/GEEFillAmmo.h"
#include "GAS/AttributeSet/WeaponAttributeSet.h"
#include "AbilitySystemComponent.h"

struct FAttribCapture_FillAmmo
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Ammo);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxAmmo);
	
	FAttribCapture_FillAmmo()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWeaponAttributeSet, Ammo, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWeaponAttributeSet, MaxAmmo, Source, false);
	}
};



static const FAttribCapture_FillAmmo& GetAttributeCapture_FillAmmo()
{
	static FAttribCapture_FillAmmo Capture;
	return Capture;
}

UGEEFillAmmo::UGEEFillAmmo()
{
	RelevantAttributesToCapture.Add(GetAttributeCapture_FillAmmo().AmmoDef);
	RelevantAttributesToCapture.Add(GetAttributeCapture_FillAmmo().MaxAmmoDef);
}

void UGEEFillAmmo::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	
	float MaxAmmo = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(GetAttributeCapture_FillAmmo().MaxAmmoProperty);
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture().MaxAmmoDef,  FAggregatorEvaluateParameters(), MaxAmmo);
	
	float CurrentAmmo = ExecutionParams.GetSourceAbilitySystemComponent()->GetNumericAttribute(GetAttributeCapture_FillAmmo().AmmoProperty);
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture().AmmoDef,  FAggregatorEvaluateParameters(), CurrentAmmo);
	
	
	float AmmoToAdd = MaxAmmo - CurrentAmmo;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture_FillAmmo().AmmoProperty, EGameplayModOp::Additive, AmmoToAdd));
}
