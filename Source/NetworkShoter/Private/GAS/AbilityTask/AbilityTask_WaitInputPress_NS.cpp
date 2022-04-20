// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/AbilityTask_WaitInputPress_NS.h"
#include "NSAbilitySystemComponent.h"

UAbilityTask_WaitInputPress_NS* UAbilityTask_WaitInputPress_NS::AbilityTask_WaitInputPress_NS(
	UGameplayAbility* OwningAbility, const FGameplayTagContainer& AbilityTags, bool TriggerOnce)
{
	auto Task = NewAbilityTask<UAbilityTask_WaitInputPress_NS>(OwningAbility);
	
	Task->bTriggerOnce = TriggerOnce;
	Task->Tags = AbilityTags;
	
	return Task;
}

void UAbilityTask_WaitInputPress_NS::Activate()
{
	Super::Activate();

	if (Tags.IsEmpty()) return;
	
	auto NSAbilitySystem = Cast<UNSAbilitySystemComponent>(AbilitySystemComponent);
	if (NSAbilitySystem)
	{
		NSAbilitySystem->InputPress.AddUObject(this, &UAbilityTask_WaitInputPress_NS::OnInputPress);
	}
}

void UAbilityTask_WaitInputPress_NS::OnDestroy(bool bInOwnerFinished)
{
	auto NSAbilitySystem = Cast<UNSAbilitySystemComponent>(AbilitySystemComponent);
	if (NSAbilitySystem)
	{
		NSAbilitySystem->InputPress.Remove(InputPressDelegateHandle);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitInputPress_NS::OnInputPress(const FGameplayTagContainer& TagContainer)
{
	if (TagContainer.HasAll(Tags))
	{
		InputPress.Broadcast();
		if (bTriggerOnce)
		{
			EndTask();
		}
	}
}
