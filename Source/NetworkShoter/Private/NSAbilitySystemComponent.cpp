// Fill out your copyright notice in the Description page of Project Settings.


#include "NSAbilitySystemComponent.h"

void UNSAbilitySystemComponent::SendInputReleaseToAbility(const FGameplayTagContainer& GameplayTagContainer, bool bReplicateOnServer)
{
	InputRelease.Broadcast(GameplayTagContainer);

	if (bReplicateOnServer)
		Server_SendInputReleaseToAbility(GameplayTagContainer);
}

void UNSAbilitySystemComponent::SendInputPressToAbility(const FGameplayTagContainer& GameplayTagContainer, bool bReplicateOnServer)
{
	InputPress.Broadcast(GameplayTagContainer);
	
	TryActivateAbilitiesByTag(GameplayTagContainer);

	if (bReplicateOnServer)
		Server_SendInputPressToAbility(GameplayTagContainer);
}

int32 UNSAbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	//if no payload try activate not event way ( event way cant activate local predicted ability on server )
	
	if (!HasPayload(Payload))
	{
		int32 TriggeredCount = 0;
		if (GameplayEventTriggeredAbilities.Contains(EventTag))
		{
			TArray<FGameplayAbilitySpecHandle> TriggeredAbilityHandles = GameplayEventTriggeredAbilities[EventTag];

			for (const FGameplayAbilitySpecHandle& AbilityHandle : TriggeredAbilityHandles)
			{
				if (TryActivateAbility(AbilityHandle))
				{
					TriggeredCount++;
				}
			}
		}
		return TriggeredCount;
	}
	
	return Super::HandleGameplayEvent(EventTag, Payload);
}

bool UNSAbilitySystemComponent::HasPayload(const FGameplayEventData* Payload)
{
	if (!Payload) return false;
	if (!Payload->Instigator &&
		!Payload->Target &&
		!Payload->TargetData.IsValid(0))
	{
		return false;
	}

	return true;
}

void UNSAbilitySystemComponent::Server_SendInputReleaseToAbility_Implementation(
	const FGameplayTagContainer& GameplayTagContainer)
{
	SendInputReleaseToAbility(GameplayTagContainer);
}

void UNSAbilitySystemComponent::Server_SendInputPressToAbility_Implementation(
	const FGameplayTagContainer& GameplayTagContainer)
{
	SendInputPressToAbility(GameplayTagContainer);
}

/* or use todo ability inputs, and use replicate input directly */