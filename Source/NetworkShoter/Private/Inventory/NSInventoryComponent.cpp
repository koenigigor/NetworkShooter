// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/NSInventoryComponent.h"

#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Inventory/Fragment_ItemInfo.h"


#pragma region FInventoryList
void FInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const auto& Index : RemovedIndices)
	{
		auto& Entry = Entries[Index];
		ensure(Entry.Item);
		AccelerationMapRemoveValue(Entry.Item->GetItemDefinition(), Entry.StackCount);

		const int32 RemovedCount = Entry.StackCount;
		Entry.LastObservedCount = Entry.StackCount;
		Entry.StackCount = 0;

		InventoryComponent->InventoryUpdate.Broadcast(Entry, EInventoryUpdateType::RemoveItem);
	}
}

void FInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		auto& Entry = Entries[Index];
		ensure(Entry.Item); //sometimes object can not replicate in time

		int32& CurrentValue = DefCountMap.FindOrAdd(Entry.Item->GetItemDefinition());
		CurrentValue += Entry.StackCount;

		Entry.LastObservedCount = Entry.StackCount;

		InventoryComponent->InventoryUpdate.Broadcast(Entry, EInventoryUpdateType::NewItem);
	}
}

void FInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const auto& Index : ChangedIndices)
	{
		auto& Entry = Entries[Index];
		const int32 Delta = Entry.StackCount - Entry.LastObservedCount;

		int32& CurrentValue = DefCountMap.FindOrAdd(Entry.Item->GetItemDefinition());
		CurrentValue += Delta;

		InventoryComponent->InventoryUpdate.Broadcast(Entry, Delta > 0 ? EInventoryUpdateType::IncreaseCount : EInventoryUpdateType::DecreaseCount);

		Entry.LastObservedCount = Entry.StackCount;
	}
}

void FInventoryList::AddEntry(TSubclassOf<UNSItemDefinition> Definition, int32 Count)
{
	AddEntry(CreateInstance(Definition), Count);
}

void FInventoryList::AddEntry(UNSItemInstance* Item, int32 Count)
{
	if (IsStackable(Item->GetItemDefinition()) && DefCountMap.Contains(Item->GetItemDefinition()))
	{
		auto& Stack = FindItem_Mutable(Item->GetItemDefinition());
		Stack.StackCount += Count;
		Item->MarkAsGarbage();

		DefCountMap[Item->GetItemDefinition()] += Count;
		InventoryComponent->InventoryUpdate.Broadcast(Stack, EInventoryUpdateType::IncreaseCount);

		MarkItemDirty(Stack);
		return;
	}

	if (Count > 1 && !IsStackable(Item->GetItemDefinition()))
	{
		//recursive add 1 item if try add more then 1 unstackable item
		AddEntry(CreateInstance(Item->GetItemDefinition()), Count - 1);
		Count = 1;
	}

	auto& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = Item;
	Entry.StackCount = Count;

	int32& CurrentCount = DefCountMap.FindOrAdd(Item->GetItemDefinition());
	CurrentCount += Count;

	InventoryComponent->InventoryUpdate.Broadcast(Entry, EInventoryUpdateType::NewItem);

	MarkItemDirty(Entry);
}

bool FInventoryList::RemoveEntry(TSubclassOf<UNSItemDefinition> Definition, TArray<FInventoryEntry>& RemovedEntries, int32 Count, bool bExactCount)
{
	if (Count == 0 || !Definition || !DefCountMap.Contains(Definition)) return false; //no item
	if (bExactCount && DefCountMap[Definition] < Count) return false;                 //no enough count

	int32 CountToRemove = FMath::Min(Count, DefCountMap[Definition]);
	for (auto It = Entries.CreateIterator(); It && CountToRemove != 0; ++It)
	{
		const FInventoryEntry& Entry = *It;
		if (Entry.Item->GetItemDefinition() != Definition) continue; //next iteration

		RemoveEntryInternal(It, CountToRemove, RemovedEntries);
	}
	return true;
}

bool FInventoryList::RemoveEntry(UNSItemInstance* Item, TArray<FInventoryEntry>& RemovedEntries, int32 Count, bool bExactCount)
{
	if (Count == 0 || !Item) return false; //no item
	const auto Definition = Item->GetItemDefinition();
	if (!DefCountMap.Contains(Definition)) return false;

	if (bExactCount && DefCountMap[Definition] < Count) return false; //no enough count

	int32 CountToRemove = FMath::Min(Count, DefCountMap[Definition]);
	for (auto It = Entries.CreateIterator(); It && CountToRemove != 0; ++It)
	{
		const FInventoryEntry& Entry = *It;
		if (Entry.Item == Item)
		{
			RemoveEntryInternal(It, CountToRemove, RemovedEntries);
		}
	}

	if (RemovedEntries.Num() == 0) return false; //target item instance not found
	if (CountToRemove == 0) return true;

	return RemoveEntry(Item->GetItemDefinition(), RemovedEntries, Count, bExactCount);
}

void FInventoryList::RemoveEntryInternal(
	TIndexedContainerIterator<TArray<FInventoryEntry>, FInventoryEntry, int>& Iterator, int32& CountToRemove, TArray<FInventoryEntry>& RemovedEntries)
{
	FInventoryEntry& Entry = *Iterator;

	const auto Definition = Entry.Item->GetItemDefinition();
	if (Entry.StackCount <= CountToRemove)
	{
		AccelerationMapRemoveValue(Definition, Entry.StackCount);

		auto EntryForDelegate = FInventoryEntry(Entry.Item, 0);
		EntryForDelegate.LastObservedCount = Entry.StackCount;
		InventoryComponent->InventoryUpdate.Broadcast(EntryForDelegate, EInventoryUpdateType::RemoveItem);

		RemovedEntries.Add(Entry);
		CountToRemove -= Entry.StackCount;

		Iterator.RemoveCurrent();
		MarkArrayDirty();
	}
	else
	{
		//Remove part of stack
		const int32 NewCount = Entry.StackCount - CountToRemove;
		Entry.StackCount = NewCount;

		const auto RemovedEntry = FInventoryEntry(CreateInstance(Definition), CountToRemove);

		AccelerationMapRemoveValue(Definition, CountToRemove);
		InventoryComponent->InventoryUpdate.Broadcast(Entry, EInventoryUpdateType::DecreaseCount);

		RemovedEntries.Add(RemovedEntry);
		CountToRemove = 0;

		MarkItemDirty(Entry);
	}
}

void FInventoryList::AccelerationMapRemoveValue(TSubclassOf<UNSItemDefinition> Definition, int32 Value)
{
	if (Value <= 0) return;

	DefCountMap[Definition] -= Value;
	if (DefCountMap[Definition] == 0)
		DefCountMap.Remove(Definition);
}

FInventoryEntry& FInventoryList::FindItem_Mutable(TSubclassOf<UNSItemDefinition> Definition)
{
	for (auto& Entry : Entries)
	{
		if (Entry.Item && Entry.Item->GetItemDefinition() == Definition)
		{
			return Entry;
		}
	}

	ensure(false);
	__assume(false); //unreachable
	//return ???;
}

FInventoryEntry FInventoryList::FindItem(TSubclassOf<UNSItemDefinition> Definition)
{
	if (DefCountMap.Contains(Definition))
		for (const auto& Entry : Entries)
		{
			if (Entry.Item && Entry.Item->GetItemDefinition() == Definition)
			{
				return Entry;
			}
		}

	return FInventoryEntry();
}

FInventoryEntry FInventoryList::FindItem(UNSItemInstance* Instance)
{
	for (const auto& Entry : Entries)
	{
		if (Entry.Item == Instance) return Entry;
	}

	return FInventoryEntry();
}

bool FInventoryList::IsStackable(TSubclassOf<UNSItemDefinition> Definition) const
{
	const auto ItemInfo = GetDefault<UNSItemDefinition>(Definition)->FindFragmentByClass<UFragment_ItemInfo>(); //

	if (ItemInfo && ItemInfo->bIsStackable)
		return true;
	return false;
}

UNSItemInstance* FInventoryList::CreateInstance(TSubclassOf<UNSItemDefinition> Definition)
{
	check(InventoryComponent);

	UObject* Outer = InventoryComponent->GetOwner() ? (UObject*)InventoryComponent->GetOwner() : (UObject*)InventoryComponent;

	return GetDefault<UNSItemDefinition>(Definition)->CreateInstance(Outer);
}

#pragma endregion FInventoryList


UNSInventoryComponent::UNSInventoryComponent()
	: InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UNSInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, InventoryList, COND_OwnerOnly);
}

bool UNSInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventoryEntry& Entry : InventoryList.Entries)
	{
		UNSItemInstance* Instance = Entry.Item;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UNSInventoryComponent::AddItem_Definition(TSubclassOf<UNSItemDefinition> Definition, int32 Count)
{
	InventoryList.AddEntry(Definition, Count);
}

void UNSInventoryComponent::AddItem_Instance(UNSItemInstance* Item, int32 Count)
{
	InventoryList.AddEntry(Item, Count);
}

bool UNSInventoryComponent::RemoveItem(TSubclassOf<UNSItemDefinition> Definition, TArray<FInventoryEntry>& RemovedItems, int32 Count, bool bDestroy,
	bool bExactCount)
{
	const auto Result = InventoryList.RemoveEntry(Definition, RemovedItems, Count, bExactCount);

	if (bDestroy)
	{
		for (auto& Stack : RemovedItems)
		{
			Stack.Item->MarkAsGarbage();
		}
		RemovedItems.Empty();
	}

	return Result;
}

//todo change signature for simplify use (UNSItemInstance* Item, int32 Count, bool bExactCount = true, bool bDestroy = false, TArray<FInventoryEntry>* RemovedItems = nullptr)
bool UNSInventoryComponent::RemoveItem(UNSItemInstance* Item, TArray<FInventoryEntry>& RemovedItems, int32 Count, bool bDestroy,
	bool bExactCount)
{
	if (!Item) return false;

	const auto Result = InventoryList.RemoveEntry(Item, RemovedItems, Count, bExactCount);

	if (bDestroy)
	{
		for (auto& Stack : RemovedItems)
		{
			Stack.Item->MarkAsGarbage();
		}
		RemovedItems.Empty();
	}

	return Result;
}

bool UNSInventoryComponent::RemoveItems(const TMap<TSubclassOf<UNSItemDefinition>, int32>& Items, TArray<FInventoryEntry>& RemovedItems, bool bDestroy,
	bool bExactCount)
{
	if (bExactCount)
	{
		if (!CheckItems(Items)) return false;
	}

	for (const auto& Item : Items)
	{
		RemoveItem(Item.Key, RemovedItems, Item.Value, bDestroy, bExactCount);
	}
	return true;
}

bool UNSInventoryComponent::RemoveItemInstance(UNSItemInstance* Item, TArray<FInventoryEntry>& RemovedItems, int32 Count, bool bDestroy,
	bool bExactCount)
{
	return RemoveItem(Item, RemovedItems, Count, bDestroy, bExactCount);
}

FInventoryEntry UNSInventoryComponent::FindItem(TSubclassOf<UNSItemDefinition> Definition)
{
	return InventoryList.FindItem(Definition);
}

bool UNSInventoryComponent::CheckItems(const TMap<TSubclassOf<UNSItemDefinition>, int32>& Items)
{
	for (const auto& Item : Items)
	{
		if (GetTotalCount(Item.Key) < Item.Value) return false;
	}
	return true;
}

int32 UNSInventoryComponent::GetTotalCount(TSubclassOf<UNSItemDefinition> Definition)
{
	return InventoryList.DefCountMap.Contains(Definition) ? InventoryList.DefCountMap[Definition] : 0;
}

int32 UNSInventoryComponent::GetCount(UNSItemInstance* Instance)
{
	return InventoryList.FindItem(Instance).StackCount;
}

TArray<FInventoryEntry> UNSInventoryComponent::GetInventory()
{
	TArray<FInventoryEntry> Results;
	if (InventoryList.Entries.Num() == 0) return Results;

	Results.Reserve(InventoryList.Entries.Num());
	for (const auto& Entry : InventoryList.Entries)
	{
		Results.Add(Entry);
	}
	return Results;
}

void UNSInventoryComponent::AddItemFromEquipment(UNSItemInstance* Item)
{
	AddItem_Instance(Item);

	//if (AddItem_Instance(Item))	TODO
	return;

	//drop item to world
	//spawn item container
}
