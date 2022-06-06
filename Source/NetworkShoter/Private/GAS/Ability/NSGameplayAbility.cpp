// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/NSGameplayAbility.h"

#include "Inventory/NSInventoryComponent.h"

bool UNSGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   FGameplayTagContainer* OptionalRelevantTags) const
{
	const auto bParentCost = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);

	if (!bParentCost || ItemCost.Num() == 0) return bParentCost;
	
	const auto Inventory = GetOwningActorFromActorInfo()->FindComponentByClass<UNSInventoryComponent>();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory not find fot check ability %s cost"), *GetName())
		return false;
	}

	return Inventory->CheckItems(ItemCost);
}

void UNSGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	const auto Inventory = GetOwningActorFromActorInfo()->FindComponentByClass<UNSInventoryComponent>();
	TArray<FInventoryEntry> RemovedItems;
	Inventory->RemoveItems(ItemCost, RemovedItems, true, false);
}
