// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/NSItemSlider.h"

#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"
#include "Net/UnrealNetwork.h"


bool FItemOnSlider::IsValid() const
{
	return Instance || Definition;
}

void FItemOnSlider::Invalidate()
{
	Definition = nullptr;
	Instance = nullptr;
	Count = 0;
}


UNSItemSlider::UNSItemSlider()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

void UNSItemSlider::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		//DestroyComponent(); //wanna keep component only for owner
		return;
	}

	Items.SetNum(MaxSlots);
	
	Inventory = GetOwner()->FindComponentByClass<UNSInventoryComponent>();

	ensure(Inventory);

	Inventory->ItemUpdate.AddUObject(this, &ThisClass::OnItemUpdate);
}

void UNSItemSlider::Next(bool bUp, bool bOnlyValidSlot)
{
	if (Items.Num() == 0) return;
	if (Items.Num() == 1 && ActiveSlot == 0) return;

	const int32 StartSearch = ActiveSlot;
	int32 PreviousSlot;
	do
	{
		int32 NextSlot = bUp ? ActiveSlot + 1 : ActiveSlot - 1;
	
		if (NextSlot > Items.Num()-1) { NextSlot = 0; }
		if (NextSlot < 0) { NextSlot = Items.Num()-1; }

		PreviousSlot = ActiveSlot;
		ActiveSlot = NextSlot;
	} while (bOnlyValidSlot && !Items[ActiveSlot].IsValid() && StartSearch != ActiveSlot);

	if (CachedSlot != ActiveSlot)
	{
		if (Items.IsValidIndex(CachedSlot) && Items[CachedSlot].Count <= 0)
		{
			Items[CachedSlot].Invalidate();
		}
		
		CachedSlot = INDEX_NONE;
	}
	
	OnSlotSelected(PreviousSlot);
	ActiveSlotUpdateDelegate.Broadcast();
}

void UNSItemSlider::OnSlotSelected_Implementation(int32 PreviousSlot)
{
}

bool UNSItemSlider::IsItemMustBeAddedOnSlider_Implementation(UNSItemInstance* Item)
{
	return true;
}

void UNSItemSlider::OnItemUpdate(FInventoryEntry Entry)
{
	if (!Entry.IsValid() || !IsItemMustBeAddedOnSlider(Entry.Item)) return;
	const auto Definition = Entry.Item->GetItemDefinition();

	//if item already on slider
	//update count
	if (const auto Index = FindIndexOnSlider(Entry.Item); Index != INDEX_NONE)
	{
		Items[Index].Count = GetNewCount(Entry);

		if (Items[Index].Count <= 0)
		{
			if (bCacheActiveRemovedSlot && Index == ActiveSlot)
			{
				
				CachedSlot = Index;
			}
			else
			{
				Items[Index].Invalidate();
				if (!Items[ActiveSlot].IsValid() && bChangeActiveSlotOnInvalidate) Next(false);
			}
		}
		
		SliderUpdateDelegate.Broadcast();
		return;
	} 
	
	//add new item
	if (const auto Index = GetFirstFreeIndex(); Index != INDEX_NONE)
	{
		Items[Index] = FItemOnSlider(Definition, Entry.Item, Entry.StackCount);

		//if current slot not valid, select first added
		if (!Items.IsValidIndex(ActiveSlot) || !Items[ActiveSlot].IsValid()) Next();
		
		SliderUpdateDelegate.Broadcast();
	}
}

int32 UNSItemSlider::FindIndexOnSlider(const UNSItemInstance* Item) const
{
	if (SliderType == ESliderType::ByItemDefinition)
	{
		return Items.IndexOfByPredicate([&](const FItemOnSlider& Entry)
		{
			return Entry.Definition == Item->GetItemDefinition();
		});
	}
	
	if (SliderType == ESliderType::ByItemInstance)
	{
		return Items.IndexOfByPredicate([&](const FItemOnSlider& Entry)
		{
			return Entry.Instance == Item;
		});
	}
		
	return INDEX_NONE;
}

int32 UNSItemSlider::GetNewCount(const FInventoryEntry& Entry) const
{
	if (SliderType == ESliderType::ByItemDefinition)
	{
		return Inventory->GetTotalCount(Entry.Item->GetItemDefinition());
	}
	
	if (SliderType == ESliderType::ByItemInstance)
	{
		return Entry.StackCount;
	}
	
	return 0;
}

int32 UNSItemSlider::GetFirstFreeIndex() const
{
	return Items.IndexOfByPredicate([&](const FItemOnSlider& Entry)
		{
			return !Entry.IsValid();
		});
}
