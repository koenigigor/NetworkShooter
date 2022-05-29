// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/NSItemInstance.h"
#include "Inventory/NSItemDefinition.h"

#include "Net/UnrealNetwork.h"

void UNSItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, ItemDefinition);
	DOREPLIFETIME(ThisClass, ItemAttributes);
}

#pragma region ItemAttributes (Tag - Value)
void UNSItemInstance::AddStatTagValue(FGameplayTag Tag, float Value)
{
	ItemAttributes.AddAttribute(Tag, Value);
}

void UNSItemInstance::RemoveStatTagValue(FGameplayTag Tag, float Value)
{
	ItemAttributes.RemoveAttribute(Tag, Value);
}

float UNSItemInstance::GetStatTagStackValue(FGameplayTag Tag) const
{
	return ItemAttributes.GetAttribute(Tag);
}

bool UNSItemInstance::HasAttributeTag(FGameplayTag Tag) const
{
	return ItemAttributes.ContainAttribute(Tag);
}
#pragma endregion

UNSInventoryItemFragment* UNSItemInstance::FindFragmentByClass(
	TSubclassOf<UNSInventoryItemFragment> FragmentClass) const
{
	if (!GetItemDefinition() || !FragmentClass) return nullptr;
	
	return GetDefault<UNSItemDefinition>(GetItemDefinition())->FindFragmentByClass(FragmentClass);
}

