// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/AbilityTask_WaitInputRelease_NS.h"

#include "NSAbilitySystemComponent.h"

UAbilityTask_WaitInputRelease_NS* UAbilityTask_WaitInputRelease_NS::AbilityTask_WaitInputRelease_NS(
	UGameplayAbility* OwningAbility, const FGameplayTagContainer& AbilityTags, bool TriggerOnce)
{
	auto Task = NewAbilityTask<UAbilityTask_WaitInputRelease_NS>(OwningAbility);
	
	Task->bTriggerOnce = TriggerOnce;
	Task->Tags = AbilityTags;
	
	return Task;
}

void UAbilityTask_WaitInputRelease_NS::Activate()
{
	Super::Activate();

	if (Tags.IsEmpty()) return;
	
	auto NSAbilitySystem = Cast<UNSAbilitySystemComponent>(AbilitySystemComponent);
	if (NSAbilitySystem)
	{
		NSAbilitySystem->InputRelease.AddDynamic(this, &UAbilityTask_WaitInputRelease_NS::OnInputRelease);
	}
}

void UAbilityTask_WaitInputRelease_NS::OnInputRelease(const FGameplayTagContainer& TagContainer)
{
	if (TagContainer.HasAll(Ability->AbilityTags))
	{
		InputRelease.Broadcast();
		if (bTriggerOnce)
		{
			EndTask();
		}
	}
}
