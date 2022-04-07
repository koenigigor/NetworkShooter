// Fill out your copyright notice in the Description page of Project Settings.


#include "NSAbilitySystemComponent.h"

void UNSAbilitySystemComponent::SendInputReleaseToAbility_Implementation(const FGameplayTagContainer& GameplayTagContainer)
{
	InputRelease.Broadcast(GameplayTagContainer);
}

void UNSAbilitySystemComponent::SendInputPressToAbility_Implementation(const FGameplayTagContainer& GameplayTagContainer)
{
	InputPress.Broadcast(GameplayTagContainer);
	
	TryActivateAbilitiesByTag(GameplayTagContainer);
}