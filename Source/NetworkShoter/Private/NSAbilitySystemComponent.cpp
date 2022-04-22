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