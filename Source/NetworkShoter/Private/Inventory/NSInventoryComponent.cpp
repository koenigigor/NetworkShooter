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

		//auto Defenition = Entry.Item->GetItemDefinition();
		//DefCountMap.FindOrAdd(Entry.Item->GetItemDefinition());
		//TagToCountMap.Add(Item.Tag, Item.Value);

		//todo this add is work?
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
		int32 Delta = Entry.StackCount - Entry.LastObservedCount;

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
	UE_LOG(LogTemp, Warning, TEXT("Start Add ItemInst in Inventory list"))
	
	if (IsStackable(Item->GetItemDefinition()) && DefCountMap.Contains(Item->GetItemDefinition()))
	{
		auto& Stack = FindItem_Mutable(Item->GetItemDefinition());
		Stack.StackCount += Count;		
		Item->DestroyObject();
		
		MarkItemDirty(Stack);
		return;
	}
	
	auto& Entry = Entries.AddDefaulted_GetRef();
	Entry.Item = Item;
	Entry.StackCount = Count;
	int32& CurrentCount = DefCountMap.FindOrAdd(Item->GetItemDefinition());
	UE_LOG(LogTemp, Warning, TEXT("Current item Count: %d"), CurrentCount)
	
	CurrentCount += Count;
	UE_LOG(LogTemp, Warning, TEXT("New item Count: %d"), CurrentCount)
	UE_LOG(LogTemp, Warning, TEXT("In Map Count: %d"), DefCountMap.FindOrAdd(Item->GetItemDefinition()))

	MarkItemDirty(Entry);
}

TArray<FInventoryEntry> FInventoryList::RemoveEntry(TSubclassOf<UNSItemDefinition> Definition, int32 Count, bool bExactCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Start removind inventory entery"))
	
	TArray<FInventoryEntry> Result;

	if (Count == 0 || !Definition || !DefCountMap.Contains(Definition)) return Result;
	
	if (bExactCount)
	{
		if (DefCountMap[Definition] < Count)
		{
			UE_LOG(LogTemp, Warning, TEXT("cant remove, dont have this count item"))
			return Result;
		}
	}

	int32 CountToRemove = FMath::Min(Count, DefCountMap[Definition]);
	for (auto It = Entries.CreateIterator(); It && CountToRemove != 0; ++It) 
	{
		FInventoryEntry& Entry = *It;
		if (Entry.Item->GetItemDefinition() != Definition) continue;

		if (Entry.StackCount <= CountToRemove)
		{
			UE_LOG(LogTemp, Warning, TEXT("removing, stack"))
			
			It.RemoveCurrent();

			AccelerationMapRemoveValue(Definition, Entry.StackCount);
			
			Result.Add(Entry);
			CountToRemove -= Entry.StackCount;
			
			MarkArrayDirty();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("removing, breack stack"))
			
			const int32 NewCount = Entry.StackCount - CountToRemove;
			Entry.StackCount = NewCount;

			AccelerationMapRemoveValue(Definition, CountToRemove);
			
			Result.Add(FInventoryEntry(CreateInstance(Definition), CountToRemove));
			CountToRemove = 0;

			MarkItemDirty(Entry);
		}
	}
	return Result;
}

TArray<FInventoryEntry> FInventoryList::RemoveEntry(UNSItemInstance* Item, int32 Count, bool bExactCount)
{
	//todo
	return RemoveEntry(Item->GetItemDefinition(), Count, bExactCount);
}

void FInventoryList::AccelerationMapRemoveValue(TSubclassOf<UNSItemDefinition> Definition, int32 Value)
{
	if (Value<=0)
	{
		UE_LOG(LogTemp, Warning, TEXT("FInventoryList::AccelerationMapRemoveValue Value cant be negative"))
		return;
	}
	
	DefCountMap[Definition] -= Value;
	if (DefCountMap[Definition] == 0)
		DefCountMap.Remove(Definition);

	UE_LOG(LogTemp, Warning, TEXT("Acceleration map value changed"))
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
	UE_LOG(LogTemp, Warning, TEXT("Start find item"))
	
	if (DefCountMap.Contains(Definition))
	for (const auto& Entry : Entries)
	{
		if (Entry.Item && Entry.Item->GetItemDefinition() == Definition)
		{
			return Entry;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("item not found"))
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

	UE_LOG(LogTemp, Warning, TEXT("FInventoryList::CreateInstance Start creating new Item Instance"))
	
	auto InstanceType = GetDefault<UNSItemDefinition>(Definition)->InstanceType;
	if (!InstanceType)
	{
		InstanceType = UNSItemInstance::StaticClass();
	}
	
	auto NewInstance = NewObject<UNSItemInstance>(InventoryComponent->GetOwner(), InstanceType);
	NewInstance->SetItemDef(Definition);

	UE_LOG(LogTemp, Warning, TEXT("FInventoryList::CreateInstance Created new Item Instance"))
	
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
		Stack.Item->DestroyObject();
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





