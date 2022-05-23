// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/NSGameplayAbility_FromEquipment.h"
#include "AbilitySystemComponent.h"
#include "Equipment/NSEquipmentInstance.h"

UNSEquipmentInstance* UNSGameplayAbility_FromEquipment::GetAssociatedEquipment()
{
	if (auto Spec = GetCurrentAbilitySpec())
		return Cast<UNSEquipmentInstance>(Spec->SourceObject);
	
	return nullptr;
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

bool UNSGameplayAbility_FromEquipment::ItemStillEquipped()
{
	/*
	auto Equipment = GetAvatarActorFromActorInfo()->GetComponentByClass(UNSEquipmentComponent::StaticClass());
	if (Equipment)
	{
		Equipment->
	}*/

	
	return true;
}
