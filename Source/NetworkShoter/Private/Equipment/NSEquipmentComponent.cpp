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
		AccelerationMap.Remove(Entry.EquipmentInstance);
		SlotMap.Remove(Entry.Slot);
	} 
}

void FNSEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		auto& Entry = Entries[Index];
		ensure(Entry.EquipmentInstance);
		AccelerationMap.Add(Entry.EquipmentInstance, Entry.ItemInstance);
		SlotMap.Add(Entry.Slot, Entry);		//it must override previous value todo check
	} 
}

void FNSEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	UE_LOG(LogTemp, Error, TEXT("Equipment PostReplicatedAdd, think it imposible root"))
	for (const auto& Index : ChangedIndices)
	{
		auto& Entry = Entries[Index];
		ensure(Entry.EquipmentInstance);
		AccelerationMap[Entry.EquipmentInstance] = Entry.ItemInstance;
		SlotMap.Add(Entry.Slot, Entry);
	} 
}

void FNSEquipmentList::AddEntry(FNSEquipmentEntry Entry)
{
	Entries.Add(Entry);
	
	AccelerationMap.Add(Entry.EquipmentInstance, Entry.ItemInstance);
	SlotMap.Add(Entry.Slot, Entry);
	
	MarkItemDirty(Entry);
}

FNSEquipmentEntry FNSEquipmentList::RemoveEntry(const UNSEquipmentInstance* Instance)
{
	if (AccelerationMap.Contains(Instance))
	for (auto It = Entries.CreateIterator(); It; ++It)
	{
		FNSEquipmentEntry Entry = *It;
		if (Entry.EquipmentInstance != Instance) continue; //next iteration

		AccelerationMap.Remove(Instance);
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

UNSEquipmentInstance* UNSEquipmentComponent::EquipItem(UNSItemInstance* Item)
{
	if (!Item) return nullptr;

	const auto Fragment = Item->FindFragmentByClass<UFragment_Equipable>();
	if (!Fragment) return nullptr;
	
	const auto Definition = NewObject<UNSEquipmentDefinition>(GetOwner(), Fragment->GetDefinitionClass());
	
	const EEquipmentSlot SlotsToEquip = SuggestSlotToEquip(Definition->Type);
	UnEquipItemInSlot(SlotsToEquip);

	//create instance
	auto EquipmentInstance = NewObject<UNSEquipmentInstance>(GetOwner(), Definition->GetInstanceType());
	EquipmentInstance->SetInstigator(GetOwner());
	for (const auto& EquipmentFragment : Definition->Fragments)
	{
		EquipmentFragment->OnEquip(EquipmentInstance);
	}
	
	EquipmentList.AddEntry(FNSEquipmentEntry(Item, EquipmentInstance, SlotsToEquip));
	
	ItemEquip.Broadcast(Definition->Type, SlotsToEquip);
	
	return EquipmentInstance;
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
