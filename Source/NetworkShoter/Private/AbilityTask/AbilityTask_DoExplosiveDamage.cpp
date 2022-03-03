// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask/AbilityTask_DoExplosiveDamage.h"


UAbilityTask_DoExplosiveDamage* UAbilityTask_DoExplosiveDamage::AbilityTask_DoExplosiveDamage(
	UGameplayAbility* OwningAbility, FVector ExplodeLocation, float ExplodeRadius, float ExplodeBaseDamage, UCurveFloat* ExplodeDamping)
{
	auto abilityTask = NewAbilityTask<UAbilityTask_DoExplosiveDamage>(OwningAbility);
	abilityTask->Location = ExplodeLocation;
	abilityTask->Radius = ExplodeRadius;
	abilityTask->BaseDamage = ExplodeBaseDamage;
	abilityTask->Damping = ExplodeDamping;
	return abilityTask;
}

void UAbilityTask_DoExplosiveDamage::Activate()
{
	Super::Activate();

	if (Location==FVector::ZeroVector || Radius <= 0.f || BaseDamage <= 0.f) { return; }

	//get overlap actors
	TArray<FOverlapResult> DamagedActors;
	bool OverlapSomething = GetWorld()->OverlapMultiByChannel(DamagedActors,
		Location,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius));

	if (!OverlapSomething){ return; }

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

		//Same actor secure
		ResultDamageMap.Add(DamagedActor.GetActor(), ResultDamage);
	} 

	//Same actor secure
	for (const auto& DamageMap : ResultDamageMap)
	{
		CalculatedDamage.Broadcast(DamageMap.Key, DamageMap.Value);
	}

	
	//apply damage
	
}
