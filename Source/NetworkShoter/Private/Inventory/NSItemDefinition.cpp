// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"

void UNSInventoryItemFragment::OnInstanceCreated(UNSItemInstance* Instance)
{
}

UNSInventoryItemFragment* UNSItemDefinition::FindFragmentByClass(
	TSubclassOf<UNSInventoryItemFragment> FragmentClass) const
{
	if (!FragmentClass) return nullptr;
	
	for (auto& Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass))
		{
			return Fragment;
		}
	}

	return nullptr;
}

const UNSInventoryItemFragment* UNSItemDefinition::FindItemDefinitionFragment(const TSubclassOf<UNSItemDefinition> ItemDef,
                                                                              const TSubclassOf<UNSInventoryItemFragment> FragmentClass)
{
	if (!ItemDef || !FragmentClass) return nullptr;

	return GetDefault<UNSItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
}

