// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/NSWeaponSliderComponent.h"

#include "Engine/ActorChannel.h"
#include "Equipment/NSEquipmentComponent.h"
#include "Equipment/NSEquipmentDefinition.h"
#include "Equipment/NSEquipmentInstance.h"
#include "Inventory/Fragment_Equipable.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemInstance.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponSlider, All, All);

#pragma region ItemsArray

void FItemSlots::AddItem(UNSItemInstance* Item)
{
	auto& Entry = Entries.Add_GetRef(Item);
	MarkItemDirty(Entry);
}

void FItemSlots::RemoveIndex(int32 Index)
{
	Entries.RemoveAt(Index);
	MarkArrayDirty();
}

void FItemSlots::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	if (OwningSlider)
	{
		OwningSlider->SliderUpdate.Broadcast();
	}
}

void FItemSlots::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{	
	if (OwningSlider)
	{
		OwningSlider->SliderUpdate.Broadcast();
	}
}

#pragma endregion ItemsArray


UNSWeaponSliderComponent::UNSWeaponSliderComponent() : Slots(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UNSWeaponSliderComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlot);
}

bool UNSWeaponSliderComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FSlotEntry& Entry : Slots.Entries)
	{
		UNSItemInstance* Instance = Entry.Item;
		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}


void UNSWeaponSliderComponent::NextWeapon_Implementation(bool bUp)
{
	ensure(Equipment);
	
	if (Slots.Num() == 0) return;
	if (Slots.Num() == 1 && ActiveSlot == 0) return;
	
	int32 NextSlot = bUp ? ActiveSlot + 1 : ActiveSlot - 1;
	
	if (NextSlot > Slots.Num()-1) { NextSlot = 0; }
	if (NextSlot < 0) { NextSlot = Slots.Num()-1; }

	//unequip previous weapon manually for it not put it in inventory
	Equipment->UnEquipItemInSlot(EEquipmentSlot::Weapon, false, false);
	const auto Weapon = Equipment->EquipItem(Slots[NextSlot]);

	UE_LOG(LogWeaponSlider, Display, TEXT("Equip item in slot %d, sussecc: %d"), NextSlot, IsValid(Weapon))
	
	ActiveSlot = NextSlot;
}

UNSItemInstance* UNSWeaponSliderComponent::RemoveWeaponFromSlider(bool bAddInStorage)
{
	if (!Equipment) return nullptr;
	if (!Slots.Entries.IsValidIndex(ActiveSlot)) return nullptr;
	
	if (Equipment->GetEquipmentBySlot(EEquipmentSlot::Weapon).ItemInstance != Slots[ActiveSlot]) return nullptr;

	Slots.RemoveIndex(ActiveSlot);

	RemovedWeapon = Equipment->UnEquipItemInSlot(EEquipmentSlot::Weapon, false, bAddInStorage);

	NextWeapon();

	SliderUpdate.Broadcast();
	
	return RemovedWeapon;
}

TArray<UNSItemInstance*> UNSWeaponSliderComponent::GetSlider()
{
	TArray<UNSItemInstance*> Output;
	for (auto Entry : Slots.Entries)
	{
		Output.Add(Entry.Item);
	}
	return Output;
}

void UNSWeaponSliderComponent::BeginPlay()
{
	Super::BeginPlay();

	Inventory = GetOwner()->FindComponentByClass<UNSInventoryComponent>();
	Equipment = GetOwner()->FindComponentByClass<UNSEquipmentComponent>();

	ensure(Inventory);
	ensure(Equipment);

	Inventory->ItemAdded.AddUObject(this, &UNSWeaponSliderComponent::AddItemOnSlider);
}

void UNSWeaponSliderComponent::AddItemOnSlider(UNSItemInstance* Item, int32 Count)
{
	if (Slots.Num() >= MaxSlots || Item == RemovedWeapon) return;
	
	const auto EquipmentFragment = Item->FindFragmentByClass<UFragment_Equipable>();
	if (!EquipmentFragment) return;		// is no equipment

	const auto EquipmentType = GetDefault<UNSEquipmentDefinition>(EquipmentFragment->GetDefinitionClass())->Type;
	if (EquipmentType != EEquipmentType::Weapon) return;	//is not weapon

	//return;
	TArray<FInventoryEntry> RemovedItems;
	if (!Inventory->RemoveItem(Item, RemovedItems)) return;
	
	Slots.AddItem(RemovedItems[0].Item);

	SliderUpdate.Broadcast();
	
	UE_LOG(LogWeaponSlider, Display, TEXT("ItemAdded on slider : "), *RemovedItems[0].Item->GetName())
}

void UNSWeaponSliderComponent::OnRep_ActiveSlot()
{
	ActiveSlotChange.Broadcast();
}
