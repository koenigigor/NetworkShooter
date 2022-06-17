// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/ShootTrace.h"

#include "Game/NSGameState.h"

void UShootTrace::MakeShoot()
{
	Super::MakeShoot();

	FHitResult Hit;
	MakeHit(Hit);
	
	const auto EffectSpec = MakeDamageEffectSpec();

	// Construct TargetData
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));

	//apply GameplayEffect
	auto ActiveEffectSpecArray = ApplyGameplayEffectSpecToTarget(
									GetCurrentAbilitySpecHandle(),
									GetCurrentActorInfo(),
									GetCurrentActivationInfo(),
	                                EffectSpec, TargetDataHandle);
	
}
