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
		
		Entry.LastObservedCount = 0; //i think in not need because entry is deleted?
	}
}

void FInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		auto& Entry = Entries[Index];
		ensure(Entry.Item);
		
		int32& CurrentValue = DefCountMap.FindOrAdd(Entry.Item->GetItemDefinition());
		CurrentValue += Entry.StackCount;

		Entry.LastObservedCount = Entry.StackCount;
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
		
		MarkItemDirty(Stack);
		return;
	}
	
	auto& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = Item;
	Entry.StackCount = Count;
	
	int32& CurrentCount = DefCountMap.FindOrAdd(Item->GetItemDefinition());
	CurrentCount += Count;

	MarkItemDirty(Entry);
}

TArray<FInventoryEntry> FInventoryList::RemoveEntry(TSubclassOf<UNSItemDefinition> Definition, int32 Count, bool bExactCount)
{
	TArray<FInventoryEntry> RemovedEntries;

	if (Count == 0 || !Definition || !DefCountMap.Contains(Definition)) return RemovedEntries;	//no item
	if (bExactCount && DefCountMap[Definition] < Count) return RemovedEntries;					//no enough count
	
	int32 CountToRemove = FMath::Min(Count, DefCountMap[Definition]);
	for (auto It = Entries.CreateIterator(); It && CountToRemove != 0; ++It) 
	{
		FInventoryEntry& Entry = *It;
		if (Entry.Item->GetItemDefinition() != Definition) continue; //next iteration

		if (Entry.StackCount <= CountToRemove)
		{
			//Remove entire stack
			It.RemoveCurrent();

			AccelerationMapRemoveValue(Definition, Entry.StackCount);
			
			RemovedEntries.Add(Entry);
			CountToRemove -= Entry.StackCount;
			
			MarkArrayDirty();
		}
		else
		{
			//Remove part of stack
			const int32 NewCount = Entry.StackCount - CountToRemove;
			Entry.StackCount = NewCount;

			AccelerationMapRemoveValue(Definition, CountToRemove);
			
			RemovedEntries.Add(FInventoryEntry(CreateInstance(Definition), CountToRemove));
			CountToRemove = 0;

			MarkItemDirty(Entry);
		}
	}
	return RemovedEntries;
}

TArray<FInventoryEntry> FInventoryList::RemoveEntry(UNSItemInstance* Item, int32 Count, bool bExactCount)
{
	//todo
	//find specified Entry, remove it first,
	//then RemoveEntry(Item->GetItemDefinition(), CountToRemove, bExactCount);
	
	return RemoveEntry(Item->GetItemDefinition(), Count, bExactCount);
}

void FInventoryList::AccelerationMapRemoveValue(TSubclassOf<UNSItemDefinition> Definition, int32 Value)
{
	if (Value<=0) return;
	
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

bool FInventoryList::IsStackable(TSubclassOf<UNSItemDefinition> Definition) const
{
	const auto ItemInfo = GetDefault<UNSItemDefinition>(Definition)->FindFragmentByClass<UFragment_ItemInfo>(); //
	
	if (!ItemInfo)	{ UE_LOG(LogTemp, Warning, TEXT("IsStackable? item info not found")) }

	if (ItemInfo && ItemInfo->bIsStackable)
		return true;
	return false;
}

UNSItemInstance* FInventoryList::CreateInstance(TSubclassOf<UNSItemDefinition> Definition)
{
	check(InventoryComponent);
	
	auto InstanceType = GetDefault<UNSItemDefinition>(Definition)->InstanceType;
	if (!InstanceType)
	{
		InstanceType = UNSItemInstance::StaticClass();
	}
	
	auto NewInstance = NewObject<UNSItemInstance>(InventoryComponent->GetOwner(), InstanceType);
	NewInstance->InitDefinition(Definition);

	return NewInstance;
}

#pragma endregion FInventoryList



UNSInventoryComponent::UNSInventoryComponent() : InventoryList(this)
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

TArray<FInventoryEntry> UNSInventoryComponent::RemoveItem(TSubclassOf<UNSItemDefinition> Definition, int32 Count, bool bDestroy,
	bool bExactCount)
{
	auto RemovedStacks = InventoryList.RemoveEntry(Definition, Count, bExactCount);

	if (!bDestroy)
		return RemovedStacks;

	
	for (auto& Stack : RemovedStacks)
	{
		Stack.Item->MarkAsGarbage();
	}
	
	return TArray<FInventoryEntry>();
}

FInventoryEntry UNSInventoryComponent::FindItem(TSubclassOf<UNSItemDefinition> Definition)
{
	return InventoryList.FindItem(Definition);
}

TArray<FInventoryEntry> UNSInventoryComponent::GetInventory()
{
	TArray<FInventoryEntry> Results;
	Results.Reserve(InventoryList.Entries.Num());
	for (const auto& Entry : InventoryList.Entries)
	{
		Results.Add(Entry);
	}
	return Results;
}





