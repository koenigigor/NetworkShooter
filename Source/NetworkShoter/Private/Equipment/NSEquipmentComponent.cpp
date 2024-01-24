// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/NSEquipmentComponent.h"

#include "Engine/ActorChannel.h"
#include "Inventory/Fragment_Equipable.h"
#include "Equipment/NSEquipmentDefinition.h"
#include "Equipment/NSEquipmentInstance.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemInstance.h"
#include "Net/UnrealNetwork.h"


void FNSEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const auto& Index : RemovedIndices)
	{
		const auto& Entry = Entries[Index];
		ensure(Entry.EquipmentInstance);
		SlotMap.Remove(Entry.Slot);
	}
}

void FNSEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		auto& Entry = Entries[Index];
		ensure(Entry.EquipmentInstance);
		//Entry.EquipmentInstance->SetInstigator(Owning);
		Entry.EquipmentInstance->SourceItem = Entry.ItemInstance; //\/ then can remove acceleration map
		SlotMap.Add(Entry.Slot, Entry);                           //it must override previous value todo check
	}
}

void FNSEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const auto& Index : ChangedIndices)
	{
		auto& Entry = Entries[Index];
		ensure(Entry.EquipmentInstance);
		SlotMap.Add(Entry.Slot, Entry);
	}
}

void FNSEquipmentList::AddEntry(FNSEquipmentEntry Entry)
{
	auto& AddedEntry = Entries.Add_GetRef(Entry);

	Entry.EquipmentInstance->SourceItem = Entry.ItemInstance;
	SlotMap.Add(Entry.Slot, Entry);

	MarkItemDirty(AddedEntry);
}

FNSEquipmentEntry FNSEquipmentList::RemoveEntry(const UNSEquipmentInstance* Instance)
{
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FNSEquipmentEntry Entry = *It;
		if (Entry.EquipmentInstance != Instance) continue; //next iteration

		SlotMap.Remove(Entry.Slot);

		It.RemoveCurrent();
		MarkArrayDirty();

		return Entry;
	}

	return FNSEquipmentEntry();
}

FNSEquipmentEntry FNSEquipmentList::GetEntryBySlot(EEquipmentSlot Slot) const
{
	return SlotMap.FindRef(Slot);
}


UNSEquipmentComponent::UNSEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UNSEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

bool UNSEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FNSEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UNSItemInstance* Item = Entry.ItemInstance;
		UNSEquipmentInstance* Equipment = Entry.EquipmentInstance;

		if (Item && IsValid(Item))
		{
			WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
		}
		if (Equipment && IsValid(Equipment))
		{
			WroteSomething |= Channel->ReplicateSubobject(Equipment, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UNSEquipmentComponent::EquipItem_Implementation(UNSItemInstance* Item)
{
	if (!Item) return;

	const auto Fragment = Item->FindFragmentByClass<UFragment_Equipable>();
	if (!Fragment) return;

	//todo CanEquip(Item); //check owner, etc...

	//if item in inventory remove it	//todo come up with something better
	if (bTryRemoveItemFromInventory)
	{
		const auto Inventory = GetOwner()->FindComponentByClass<UNSInventoryComponent>();
		ensure(Inventory);

		TArray<FInventoryEntry> RemovedItems;
		Inventory->RemoveItem(Item, RemovedItems);
	}

	const auto Definition = NewObject<UNSEquipmentDefinition>(GetOwner(), Fragment->GetDefinitionClass());

	const EEquipmentSlot SlotsToEquip = SuggestSlotToEquip(Definition->Type);
	UnEquipItemInSlot(SlotsToEquip);

	//create instance
	auto EquipmentInstance = NewObject<UNSEquipmentInstance>(GetOwner(), Definition->GetInstanceType());
	EquipmentInstance->SetInstigator(GetOwner());
	EquipmentInstance->SourceItem = Item;

	for (const auto& EquipmentFragment : Definition->Fragments)
	{
		EquipmentFragment->OnEquip(EquipmentInstance);
	}

	EquipmentList.AddEntry(FNSEquipmentEntry(Item, EquipmentInstance, SlotsToEquip));

	ItemEquip.Broadcast(Definition->Type, SlotsToEquip);

	return;
}

UNSItemInstance* UNSEquipmentComponent::UnEquipItemInSlot(EEquipmentSlot Slot, bool bDestroy, bool bPutInInventory)
{
	const auto Entry = GetEquipmentBySlot(Slot);
	if (!Entry.EquipmentInstance) return nullptr;

	return UnEquipItem(Entry.EquipmentInstance, bDestroy, bPutInInventory);
}

UNSItemInstance* UNSEquipmentComponent::UnEquipItem(UNSEquipmentInstance* Item, bool bDestroy, bool bPutInInventory)
{
	if (!Item) return nullptr;

	//remove entry from list
	const auto RemovedEntry = EquipmentList.RemoveEntry(Item);
	if (!RemovedEntry.EquipmentInstance) return nullptr; //item not found

	//send unequip to all fragments
	const auto DefinitionClass = RemovedEntry.ItemInstance->FindFragmentByClass<UFragment_Equipable>()->GetDefinitionClass();
	const auto Definition = NewObject<UNSEquipmentDefinition>(GetOwner(), DefinitionClass);
	for (const auto& Fragment : Definition->Fragments)
	{
		Fragment->OnUnequip(RemovedEntry.EquipmentInstance);
	}

	ItemUnequip.Broadcast(Item);
	Item->MarkAsGarbage();

	if (bDestroy)
	{
		RemovedEntry.ItemInstance->MarkAsGarbage();
		return nullptr;
	}

	if (bPutInInventory)
	{
		const auto Inventory = GetOwner()->FindComponentByClass<UNSInventoryComponent>();
		ensure(Inventory);
		Inventory->AddItemFromEquipment(RemovedEntry.ItemInstance);
	}

	return RemovedEntry.ItemInstance;
}

TArray<FNSEquipmentEntry> UNSEquipmentComponent::GetAllEquipment()
{
	TArray<FNSEquipmentEntry> Result;
	for (const auto& Entry : EquipmentList.Entries)
	{
		Result.Add(Entry);
	}
	return Result;
}

EEquipmentSlot UNSEquipmentComponent::SuggestSlotToEquip(EEquipmentType Type)
{
	auto Slots = UNSEquipmentLibrary::ResolveEquipmentSlot(Type);
	if (Slots.Num() == 0) return EEquipmentSlot::None;

	//try find free slot
	for (const auto& Slot : Slots)
	{
		if (!GetEquipmentBySlot(Slot).EquipmentInstance)
			return Slot; //slot is free
	}

	return Slots[0];
}
