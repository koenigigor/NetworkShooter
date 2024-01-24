// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectExecution/GECalculationShoot.h"
#include "GAS/AttributeSet/NetShooterAttributeSet.h"
#include "GAS/AttributeSet/WeaponAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GAS/AbilityBPLibrary.h"

struct FAttribCapture_CalculationShoot
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(WeaponDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	
	FAttribCapture_CalculationShoot()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWeaponAttributeSet, WeaponDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNetShooterAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNetShooterAttributeSet, Health, Target, false);
	}
};



static const FAttribCapture_CalculationShoot& GetAttributeCapture_CalculationShoot()
{
	static FAttribCapture_CalculationShoot Capture;
	return Capture;
}

UGECalculationShoot::UGECalculationShoot()
{
	RelevantAttributesToCapture.Add(GetAttributeCapture_CalculationShoot().WeaponDamageDef);
	RelevantAttributesToCapture.Add(GetAttributeCapture_CalculationShoot().ArmorDef);
	RelevantAttributesToCapture.Add(GetAttributeCapture_CalculationShoot().HealthDef);
}

void UGECalculationShoot::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	//Prep variables
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//get attributes
	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture_CalculationShoot().WeaponDamageDef, EvaluationParameters, AttackPower);
	if (AttackPower <= 0.f) { AttackPower = 1.f; }

	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture_CalculationShoot().ArmorDef, EvaluationParameters, Armor);

	float Health = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture_CalculationShoot().HealthDef, EvaluationParameters, Health);
	
	//calculation results
	float ResultDamage = AttackPower + 1; //TODO make some formula
	
	float OutArmor = Armor - ResultDamage;
	
	if (OutArmor <= 0.f)
	{
		float OutHealth = Health - fabs(OutArmor);
		
		//SetResults
		if (Armor > 0) 
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture_CalculationShoot().ArmorProperty, EGameplayModOp::Override, OutArmor));
		}
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture_CalculationShoot().HealthProperty, EGameplayModOp::Override, OutHealth));
	}
	else
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture_CalculationShoot().ArmorProperty, EGameplayModOp::Override, OutArmor));
	}
}
