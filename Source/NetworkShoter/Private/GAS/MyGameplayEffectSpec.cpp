// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MyGameplayEffectSpec.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExecutionCalculation.h"

#include "GameplayEffect.h"
#include "Game/NSGameState.h"

FGameplayEffectSpecHandle UMyGameplayEffectSpec::SetPeriod(FGameplayEffectSpecHandle SpecHandle, float Period)
{
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec)
	{
		//Spec->SetDuration(Duration, true);
		Spec->Period = Period;
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UUMyGameplayEffectSpec::SetPeriod called with invalid SpecHandle"));
	}

	return SpecHandle;
}

FGameplayEffectSpecHandle UMyGameplayEffectSpec::SetEffectCauser(const FGameplayEffectSpecHandle& SpecHandle, AActor* Causer)
{
	if (!Causer) return SpecHandle;
	
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (Spec)
	{
		//Spec->SetDuration(Duration, true);
		Spec->GetContext().AddInstigator(Spec->GetContext().GetInstigator(), Causer);
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UUMyGameplayEffectSpec::SetPeriod called with invalid SpecHandle"));
	}

	return SpecHandle;
}
