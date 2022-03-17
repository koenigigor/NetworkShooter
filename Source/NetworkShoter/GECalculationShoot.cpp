// Fill out your copyright notice in the Description page of Project Settings.


#include "GECalculationShoot.h"
#include "NetShooterAttributeSet.h"
#include "WeaponAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NSPlayerState.h"
#include "PCNetShooter.h"
#include "Game/NSGameState.h"

struct FAttribCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(WeaponDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	
	FAttribCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWeaponAttributeSet, WeaponDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNetShooterAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNetShooterAttributeSet, Health, Target, false);
	}
};



static const FAttribCapture& GetAttributeCapture()
{
	static FAttribCapture Capture;
	return Capture;
}

UGECalculationShoot::UGECalculationShoot()
{
	RelevantAttributesToCapture.Add(GetAttributeCapture().WeaponDamageDef);
	RelevantAttributesToCapture.Add(GetAttributeCapture().ArmorDef);
	RelevantAttributesToCapture.Add(GetAttributeCapture().HealthDef);
}

void UGECalculationShoot::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	//Prep variables
	UAbilitySystemComponent* SourceAbilityComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetAbilityComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* SourceActor = SourceAbilityComponent ? SourceAbilityComponent->GetAvatarActor_Direct() : nullptr;
	AActor* TargetActor = TargetAbilityComponent ? TargetAbilityComponent->GetAvatarActor_Direct() : nullptr;
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	//get attributes
	float AttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture().WeaponDamageDef, EvaluationParameters, AttackPower);
	if (AttackPower <= 0.f) { AttackPower = 1.f; }

	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture().ArmorDef, EvaluationParameters, Armor);

	float Health = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture().HealthDef, EvaluationParameters, Health);
	
	//calculation results
	float ResultDamage = AttackPower + 1; //TODO make some formula
	
	float OutArmor = Armor - ResultDamage;
	
	if (OutArmor <= 0.f)
	{
		float OutHealth = Health - fabs(OutArmor);
		
		//SetResults
		if (Armor > 0) //not trigger replication if already has been 0
		{
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture().ArmorProperty, EGameplayModOp::Override, OutArmor));
		}
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture().HealthProperty, EGameplayModOp::Override, OutHealth));
	}
	else
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture().ArmorProperty, EGameplayModOp::Override, OutArmor));
	}

/**********  TODO  **********/
	//Notify GameState about damage
	AActor* DamageCauser = nullptr;
	if (SourceActor->GetWorld() && SourceActor->GetWorld() -> GetGameState<ANSGameState>())
	{
		SourceActor->GetWorld() -> GetGameState<ANSGameState>() -> ApplyDamageInfoFromActors(SourceActor->GetInstigatorController(), TargetActor, DamageCauser, ResultDamage);
	}
}
