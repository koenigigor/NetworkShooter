// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Fragment_Usable.h"

#include "AbilitySystemComponent.h"
#include "GAS/Ability/NSGameplayAbility.h"
#include "Inventory/NSItemInstance.h"

/*
void UFragment_Usable::UseItem(UAbilitySystemComponent* ASC, UNSItemInstance* Instance)
{
	if (!ASC || !Instance) return;
	//if (!Instance->ConsumeItem()) return;	//consume 1 item
	
	const auto UsableFragment = Instance->FindFragmentByClass<UFragment_Usable>();
	if (!UsableFragment) return;
	
	auto AbilitySpec = FGameplayAbilitySpec(UsableFragment->AbilityToGrant);
	AbilitySpec.SourceObject = Instance;
	
	const FGameplayEventData* EventData = nullptr; //todo Event data?
	const auto Handle = ASC->GiveAbilityAndActivateOnce(AbilitySpec, EventData);
	//ASC OnGiveAbility Client->Activate
	ASC->SetRemoveAbilityOnEnd(Handle);
}
*/

FGameplayTag UFragment_Usable::GetAbilityActivationTag(UNSItemInstance* Instance, bool& bFound)
{
	bFound = false;
	if (!Instance) return FGameplayTag::EmptyTag;

	const auto UsableFragment = Instance->FindFragmentByClass<UFragment_Usable>();
	if (!UsableFragment) return FGameplayTag::EmptyTag;

	bFound = UsableFragment->ActivationTag.IsValid();
	return UsableFragment->ActivationTag;
}
