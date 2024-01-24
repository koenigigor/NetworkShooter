// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/NSGameplayAbility_FromEquipment.h"
#include "AbilitySystemComponent.h"
#include "Equipment/NSEquipmentComponent.h"
#include "Equipment/NSEquipmentInstance.h"
#include "Inventory/NSItemInstance.h"

UNSGameplayAbility_FromEquipment::UNSGameplayAbility_FromEquipment()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UNSEquipmentInstance* UNSGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (auto Spec = GetCurrentAbilitySpec())
		return Cast<UNSEquipmentInstance>(Spec->SourceObject);
	
	return nullptr;
}

AActor* UNSGameplayAbility_FromEquipment::GetAssociatedEquipmentActor() const
{
	return GetAssociatedEquipment()->SpawnedActors.IsValidIndex(0) ? GetAssociatedEquipment()->SpawnedActors[0] : nullptr;
}

AActor* UNSGameplayAbility_FromEquipment::GetAssociatedEquipmentActor_Ensured() const
{
	const auto EquipmentActor = GetAssociatedEquipmentActor();
	ensure(EquipmentActor);
	return EquipmentActor;
}

bool UNSGameplayAbility_FromEquipment::CheckCost(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags)) return false;
	
	if (ItemAttributeCost.Num() == 0) return true;
	
	const auto EquipmentItem = GetAssociatedEquipment();
	if (EquipmentItem && EquipmentItem->SourceItem)
	{
		for (const auto& Cost : ItemAttributeCost)
		{
			if (EquipmentItem->SourceItem->GetStatTagStackValue(Cost.Key) < Cost.Value) return false;
		}

		return true;
	}

	return false;
}

void UNSGameplayAbility_FromEquipment::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	
	const auto EquipmentItem = GetAssociatedEquipment();
	if (EquipmentItem && EquipmentItem->SourceItem)
	{
		for (const auto& Cost : ItemAttributeCost)
		{
			EquipmentItem->SourceItem->RemoveStatTagValue(Cost.Key, Cost.Value);
		} 
	}
}

bool UNSGameplayAbility_FromEquipment::ItemStillEquipped()
{
	return IsValid(GetAssociatedEquipment()); //on unequip item marks as garbage
}
