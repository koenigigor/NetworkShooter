// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"


UNSItemInstance* UNSItemDefinition::CreateInstance(UObject* Outer) const
{
	auto Instance = NewObject<UNSItemInstance>(Outer, GetInstanceType());
	
	Instance->ItemDefinition = GetClass();
	for (auto& Fragment : Fragments)
	{
		Fragment->OnInstanceCreated(Instance);
	}

	return Instance;
}

TSubclassOf<UNSItemInstance> UNSItemDefinition::GetInstanceType() const
{
	return InstanceType ? InstanceType : UNSItemInstance::StaticClass();
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

const UNSInventoryItemFragment* UNSItemDefinition::FindFragmentByClass(const TSubclassOf<UNSItemDefinition> ItemDef,
                                                                              const TSubclassOf<UNSInventoryItemFragment> FragmentClass)
{
	if (!ItemDef || !FragmentClass) return nullptr;

	return GetDefault<UNSItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
}

