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

void UNSGameplayAbility_FromEquipment::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                     const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	//wait item unequip if want
	if (bForceCancel)
	{
		if (auto Equipment = ActorInfo->AvatarActor->FindComponentByClass<UNSEquipmentComponent>())
		{
			Equipment->ItemUnequip.AddUObject(this, &UNSGameplayAbility_FromEquipment::OnUnequip);
		}
	}
}

void UNSGameplayAbility_FromEquipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&ActivationInfo) && !ItemStillEquipped())	
	{
		GetAbilitySystemComponentFromActorInfo()->ClearAbility(Handle);
	}	//after unequip still can be activated 1 time
}

void UNSGameplayAbility_FromEquipment::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (HasAuthority(&ActivationInfo) && !ItemStillEquipped())	
	{
		GetAbilitySystemComponentFromActorInfo()->ClearAbility(Handle);
	}
}

void UNSGameplayAbility_FromEquipment::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);

	//clear delegate
	if (bForceCancel)
	{
		if (auto Equipment = ActorInfo->AvatarActor->FindComponentByClass<UNSEquipmentComponent>())
		{
			Equipment->ItemUnequip.RemoveAll(this);
		}
	}
}

void UNSGameplayAbility_FromEquipment::OnUnequip(UNSEquipmentInstance* Item)
{
	if (Item == GetAssociatedEquipment())
	if (GetAvatarActorFromActorInfo()->HasAuthority())	
	{
		GetAbilitySystemComponentFromActorInfo()->ClearAbility(GetCurrentAbilitySpecHandle());
	}
}

bool UNSGameplayAbility_FromEquipment::ItemStillEquipped()
{
	return IsValid(GetAssociatedEquipment()); //on unequip item marks as garbage
}
