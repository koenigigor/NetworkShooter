// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/NSItemSlider.h"

#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"



UNSItemSlider::UNSItemSlider()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TSubclassOf<UNSItemDefinition> UNSItemSlider::GetSelectedItem()
{
	if (!Items.IsValidIndex(ActiveSlot)) return nullptr;

	return Items[ActiveSlot].Definition;
}

void UNSItemSlider::Next(bool bUp)
{
	if (Items.Num() == 0) return;
	if (Items.Num() == 1 && ActiveSlot == 0) return;
	
	int32 NextSlot = bUp ? ActiveSlot + 1 : ActiveSlot - 1;
	
	if (NextSlot > Items.Num()-1) { NextSlot = 0; }
	if (NextSlot < 0) { NextSlot = Items.Num()-1; }

	ActiveSlot = NextSlot;
	ActiveSlotUpdateDelegate.Broadcast();
}

void UNSItemSlider::BeginPlay()
{
	Super::BeginPlay();
	
	Inventory = GetOwner()->FindComponentByClass<UNSInventoryComponent>();

	ensure(Inventory);

	Inventory->ItemAdded.AddUObject(this, &ThisClass::OnItemAdded);
	Inventory->ItemRemoved.AddUObject(this, &ThisClass::OnItemRemoved);
}

void UNSItemSlider::OnItemAdded(UNSItemInstance* Item, int32 Count)
{
	if (!Item) return;
	
	const auto Definition = Item->GetItemDefinition();
	
	//if item already on slider
		//update count
	if (const auto Index = FindIndexOnSlider(Definition); Index != INDEX_NONE)
	{
		Items[Index].Count = Inventory->GetTotalCount(Definition);
		SliderUpdateDelegate.Broadcast();
		return;
	} 
	
	//add new item
	if (Items.Num() < MaxSlots && IsItemMustBeAddedOnSlider(Item))
	{
		const auto TotalCount = Inventory->GetTotalCount(Definition);

		Items.Add(FItemOnSlider(Definition, TotalCount));
		SliderUpdateDelegate.Broadcast();
	}
}

void UNSItemSlider::OnItemRemoved(UNSItemInstance* Item, int32 Count)
{
	const auto Definition = Item->GetItemDefinition();
	
	if (const auto Index = FindIndexOnSlider(Definition); Index != INDEX_NONE)
	{
		Items[Index].Count = -Count;
		if (Items[Index].Count == 0)
		{
			Items.RemoveAt(Index);
		}
		
		SliderUpdateDelegate.Broadcast();
	} 
}

bool UNSItemSlider::IsItemMustBeAddedOnSlider_Implementation(UNSItemInstance* Item)
{
	return true;
}


int32 UNSItemSlider::FindIndexOnSlider(TSubclassOf<UNSItemDefinition> Definition) const
{
	return Items.IndexOfByPredicate([&](const FItemOnSlider& Entry)
		{
			return Entry.Definition == Definition;
		});
}
