// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/NSItemInstance.h"

#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemDefinition.h"

#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogItemInstance, All, All);

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

FItemAttributeChange& UNSItemInstance::GetItemAttributeValueChangeDelegate(FGameplayTag Attribute)
{
	return ItemAttributes.GetItemAttributeValueChangeDelegate(Attribute);
}

bool UNSItemInstance::ConsumeItem(int32 Count)
{
	if (!IsValid(this)) return false;	//is already mark as garbage
	
	UE_LOG(LogItemInstance, Log, TEXT("ConsumeItem %s, Count = %d, Owner = %s"), *GetName(), Count, *GetOuter()->GetName())
	
	//item in storage
	if (const auto Owner = Cast<AActor>(GetOuter()))
	{
		if (const auto Inventory = Owner->FindComponentByClass<UNSInventoryComponent>())
		{
			TArray<FInventoryEntry> RemovedItems;
			return Inventory->RemoveItem(this, RemovedItems, Count, true, true);
		}
		
		UE_LOG(LogItemInstance, Display, TEXT("ConsumeItem but owner has no inventory"))
	}

	if (Count != 1)
	{
		UE_LOG(LogItemInstance, Display, TEXT("ConsumeItem, wrong count, item not in storage"))
		return false;
	}
	
	MarkAsGarbage();
	return true;
}

UNSInventoryItemFragment* UNSItemInstance::FindFragmentByClass(
	TSubclassOf<UNSInventoryItemFragment> FragmentClass) const
{
	if (!GetItemDefinition() || !FragmentClass) return nullptr;
	
	return GetDefault<UNSItemDefinition>(GetItemDefinition())->FindFragmentByClass(FragmentClass);
}

