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

	UE_LOG(LogTemp, Warning, TEXT("Ability From Equipment start ending"))
	if (HasAuthority(&ActivationInfo))
	//if (auto EquipmentItem = GetAssociatedEquipment())
	if (1)	//true for test //TODO
	{
		//if it last item
		UE_LOG(LogTemp, Warning, TEXT("Ability From Equipment try clear ability"))
		GetAbilitySystemComponentFromActorInfo()->ClearAbility(Handle);
		UE_LOG(LogTemp, Warning, TEXT("Ability From Equipment clear successful"))
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability From Equipment associated equipment not found"))
	}
}
