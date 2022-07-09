// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/ShootTrace.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueFunctionLibrary.h"
#include "Equipment/NSEquipmentInstance.h"		// ReSharper disable once CppUnusedIncludeDirective

void UShootTrace::MakeSingleShoot()
{
	Super::MakeSingleShoot();

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

	
	if (ImpactCue.IsValid())
	{
		auto Parameters = UGameplayCueFunctionLibrary::MakeGameplayCueParametersFromHitResult(Hit);
		Parameters.EffectCauser = GetAssociatedEquipmentActor_Ensured();
		Parameters.SourceObject = GetAssociatedEquipment();
		Parameters.TargetAttachComponent = Parameters.EffectCauser->GetRootComponent();		
		GetAbilitySystemComponentFromActorInfo_Checked() -> ExecuteGameplayCue(ImpactCue, Parameters);
	}
	///todo maybe better performance is send rpc with array vectors, and trigger local instead send big Parameters structure?
}
