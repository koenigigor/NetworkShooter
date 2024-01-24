// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/NSInventorySlider.h"

#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"


bool FSliderEntry::IsValid() const
{
	return ItemDefinition != nullptr;
}


UNSInventorySlider::UNSInventorySlider()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UNSInventorySlider::BeginPlay()
{
	Super::BeginPlay();

	Entries.SetNum(NumSlots);

	Inventory = GetOwner()->FindComponentByClass<UNSInventoryComponent>();

	if (!Inventory)
	{
		UE_LOG(LogInventorySlider, Warning, TEXT("Inventory component not found in %s"), *GetOwner()->GetName())
		return;
	}

	if (bBindToInventoryUpdate && !GetWorld()->IsNetMode(NM_DedicatedServer))
		Inventory->InventoryUpdate.AddUObject(this, &ThisClass::OnInventoryUpdate);
}

void UNSInventorySlider::AddItemInSlot(int32 Slot, TSubclassOf<UNSItemDefinition> Definition, UNSItemInstance* AssignedInstance)
{
	if (IsItemMustBeAdd(Definition))
	{
		InitSlot(Slot, Definition, AssignedInstance);
	}
}

void UNSInventorySlider::RemoveItemFromSlot(int32 Slot)
{
	InitSlot(Slot, nullptr, nullptr);
}

void UNSInventorySlider::InitSlot(int32 Slot, TSubclassOf<UNSItemDefinition> Definition, UNSItemInstance* AssignedInstance)
{
	if (!Entries.IsValidIndex(Slot)) return;

	Entries[Slot] = {Definition, AssignedInstance};

	OnUpdate.Broadcast();
}

void UNSInventorySlider::NextSlot(bool bUp)
{
	int32 ActiveIndexChecked; // Active slot with INDEX_NONE validate
	if (ActiveIndex == INDEX_NONE)
	{
		ActiveIndexChecked = bUp ? Entries.Num() - 1 : 0;
	}
	else
	{
		ActiveIndexChecked = ActiveIndex;
	}

	//iterate array for find first target slot, from current
	int32 FoundedIndex = INDEX_NONE;
	{
		int32 PreviousSlot = ActiveIndexChecked;
		bool bEnd = false;
		while (!bEnd)
		{
			//next index
			int32 IteratedSlot = bUp ? PreviousSlot + 1 : PreviousSlot - 1;
			if (IteratedSlot > Entries.Num() - 1) { IteratedSlot = 0; }
			if (IteratedSlot < 0) { IteratedSlot = Entries.Num() - 1; }

			if (IteratedSlot == ActiveIndexChecked) //we loop all array
			{
				if (ActiveIndex == INDEX_NONE)
				{
					bEnd = true; //exit after current index check
				}
				else
				{
					break; //exit 
				}
			};

			UE_LOG(LogInventorySlider, Display, TEXT("IteratedSlot = %d"), IteratedSlot)

			if (!bIterateOnlyValidSlot || bIterateOnlyValidSlot && Entries[IteratedSlot].IsValid())
			{
				FoundedIndex = IteratedSlot;
				break; //exit, index found
			}

			PreviousSlot = IteratedSlot;
		}
	}

	if (FoundedIndex == INDEX_NONE) return;

	ActiveIndex = FoundedIndex;
	OnUpdateActiveSlot.Broadcast();
	if (bUseItemOnActivateSlot) UseItem(Entries[ActiveIndex]);
}

void UNSInventorySlider::UseItemInSlot(int32 Slot)
{
	if (Entries.IsValidIndex(Slot))
		UseItem(Entries[Slot]);
}


void UNSInventorySlider::UseItem_Implementation(const FSliderEntry& Item)
{
	//
}

int32 UNSInventorySlider::GetSelectedSlot() const
{
	return ActiveIndex;
}

FSliderEntry UNSInventorySlider::GetSelectedItem() const
{
	return Entries.IsValidIndex(ActiveIndex) ? Entries[ActiveIndex] : FSliderEntry();
}

int32 UNSInventorySlider::GetCount(const FSliderEntry& Entry) const
{
	if (!Inventory) return 0;

	return bUseTotalCount
		       ? Inventory->GetTotalCount(Entry.ItemDefinition)
		       : Inventory->GetCount(Entry.AssignedInstance);
}

FSliderEntry UNSInventorySlider::GetSlot(int32 Slot, int32& Count)
{
	Count = Entries.IsValidIndex(Slot) ? GetCount(Entries[Slot]) : 0;
	return Entries.IsValidIndex(Slot) ? Entries[Slot] : FSliderEntry();
}

FSliderEntry UNSInventorySlider::GetSlot(int32 Slot)
{
	return Entries.IsValidIndex(Slot) ? Entries[Slot] : FSliderEntry();
}

UNSItemInstance* UNSInventorySlider::GetStoredInstance(int32 Slot)
{
	if (!Entries.IsValidIndex(Slot)) return nullptr;

	const auto& Entry = Entries[Slot];
	if (GetCount(Entry) <= 0) return nullptr;

	return bUseTotalCount
		   ? Inventory->FindItem(Entry.ItemDefinition).Item
		   : Entry.AssignedInstance;
}

void UNSInventorySlider::OnInventoryUpdate(FInventoryEntry InventoryEntry, EInventoryUpdateType UpdateType)
{
	if (bUseTotalCount && ItemDefAlreadyOnSlider(InventoryEntry))
	{
		OnUpdate.Broadcast();
		return;
	}

	if (UpdateType != EInventoryUpdateType::NewItem) return;

	if (!IsItemMustBeAdd(InventoryEntry.Item->GetItemDefinition())) return;

	const auto FreeSlot = GetFreeSlot(InventoryEntry);
	if (FreeSlot == INDEX_NONE) return;

	//add item in slot
	InitSlot(FreeSlot, InventoryEntry.Item->GetItemDefinition(), bUseTotalCount ? nullptr : InventoryEntry.Item);
}

bool UNSInventorySlider::IsItemMustBeAdd_Implementation(TSubclassOf<UNSItemDefinition> Item) const
{
	return false;
}

bool UNSInventorySlider::ItemDefAlreadyOnSlider(const FInventoryEntry& InventoryEntry) const
{
	if (!InventoryEntry.IsValid()) return false;

	return Entries.ContainsByPredicate([&](const FSliderEntry& SliderEntry)
	{
		return SliderEntry.ItemDefinition == InventoryEntry.Item->GetItemDefinition();
	});
}

int32 UNSInventorySlider::GetFreeSlot(const FInventoryEntry& InventoryEntry) const
{
	//check if this instance already exist on slider (like if we drop item, and pickup again, it must be stay on slider) 
	const auto ExistingIndex = Entries.IndexOfByPredicate([&](const FSliderEntry& SliderEntry)
	{
		return SliderEntry.IsValid() && SliderEntry.AssignedInstance == InventoryEntry.Item;
	});
	if (ExistingIndex != INDEX_NONE) return ExistingIndex;

	return Entries.IndexOfByPredicate([](const FSliderEntry& SliderEntry)
	{
		return !SliderEntry.IsValid();
	});
}
