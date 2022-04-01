// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/AbilityTask_DoExplosiveDamage.h"


UAbilityTask_DoExplosiveDamage* UAbilityTask_DoExplosiveDamage::AbilityTask_DoExplosiveDamage(
	UGameplayAbility* OwningAbility, FVector ExplodeLocation, float ExplodeRadius, float ExplodeBaseDamage, TSubclassOf<UGameplayEffect> DamageGameplayEffectClass, UCurveFloat* ExplodeDamping)
{
	auto abilityTask = NewAbilityTask<UAbilityTask_DoExplosiveDamage>(OwningAbility);
	abilityTask->Location = ExplodeLocation;
	abilityTask->Radius = ExplodeRadius;
	abilityTask->BaseDamage = ExplodeBaseDamage;
	abilityTask->Damping = ExplodeDamping;
	abilityTask->GameplayEffectClass = DamageGameplayEffectClass;
	
	ensure(DamageGameplayEffectClass);
	
	return abilityTask;
}

void UAbilityTask_DoExplosiveDamage::Activate()
{
	Super::Activate();

	if (Location==FVector::ZeroVector || Radius <= 0.f || BaseDamage <= 0.f)
	{
		FinishExecute.Broadcast(nullptr, nullptr);
		return;
	}

	//get overlap actors
	TArray<FOverlapResult> DamagedActors;
	bool OverlapSomething = GetWorld()->OverlapMultiByChannel(DamagedActors,
		Location,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius));

	if (!OverlapSomething)
	{
		FinishExecute.Broadcast(nullptr, nullptr);
		return;
	}

	//calculate result damage
	TMap<AActor*, float> ResultDamageMap;
	for (const auto& DamagedActor : DamagedActors)
	{
		//calculate damage percent
		float ResultDamage = BaseDamage;
		if (Damping)
		{
			float Distance = FVector::Distance(Location, DamagedActor.GetActor()->GetActorLocation());
			float DamagePercent = Damping->GetFloatValue(Distance/Radius);
			ResultDamage = BaseDamage * DamagePercent;
		}

		//Same actor protection
		ResultDamageMap.Add(DamagedActor.GetActor(), ResultDamage);
	} 

	//Apply results
	for (const auto& DamageMap : ResultDamageMap)
	{
		//make gameplay effect
		auto SpecHandle = Ability->MakeOutgoingGameplayEffectSpec(GameplayEffectClass, 1);
		if (auto Spec = SpecHandle.Data.Get())
		{
			Spec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Magnitude.Damage"), DamageMap.Value);
		}
		
		// Construct TargetData
		FGameplayAbilityTargetData_ActorArray*	TargetData = new FGameplayAbilityTargetData_ActorArray();
		TargetData->TargetActorArray.Add(DamageMap.Key);
		FGameplayAbilityTargetDataHandle Handle(TargetData);

		Damaged.Broadcast(SpecHandle, Handle);
		
		//UGameplayAbility::ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, TargetData);
	}

	FinishExecute.Broadcast(nullptr, nullptr);
}
