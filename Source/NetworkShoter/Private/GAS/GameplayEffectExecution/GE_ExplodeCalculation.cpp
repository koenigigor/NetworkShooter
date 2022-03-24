// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectExecution/GE_ExplodeCalculation.h"
#include "GAS/AttributeSet/NetShooterAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"


struct FAttributeCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	
	FAttributeCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNetShooterAttributeSet, Health, Target, false);
		
	}
};

static const FAttributeCapture& GetAttributeCapture()
{
	static FAttributeCapture Capture;
	return Capture;
}

UGE_ExplodeCalculation::UGE_ExplodeCalculation()
{
	RelevantAttributesToCapture.Add(GetAttributeCapture().HealthDef);
	ValidTransientAggregatorIdentifiers.AddTag(FGameplayTag::RequestGameplayTag("Magnitude.Damage"));
}

void UGE_ExplodeCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	/*
	//GetAttributes
	float Health = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetAttributeCapture().HealthDef, FAggregatorEvaluateParameters(), Health);
	*/
	
	float Damage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Magnitude.Damage")), true, 0);
	//float Damage = 0.f;

	//FGameplayTag::RequestGameplayTag(FName("Magnitude.Damage"));

	//Damage = Spec.GetSetByCallerMagnitude(FName("Magnitude.Damage")); // OK (ByName, may be deprecated) //
	//ExecutionParams.AttemptCalculateTransientAggregatorMagnitude(FGameplayTag::RequestGameplayTag("Magnitude.Damage"), FAggregatorEvaluateParameters(), Damage);
	
	UE_LOG(LogTemp, Warning, TEXT("Damage captured : %f"), Damage)
	Damage *= -1.f;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetAttributeCapture().HealthProperty, EGameplayModOp::Additive, Damage));
}


