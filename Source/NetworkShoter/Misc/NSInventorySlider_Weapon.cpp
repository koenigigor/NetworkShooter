// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/NSInventorySlider_Weapon.h"

#include "Equipment/NSEquipmentComponent.h"
#include "Equipment/NSEquipmentDefinition.h"
#include "Inventory/Fragment_Equipable.h"
#include "Inventory/NSItemDefinition.h"

UNSInventorySlider_Weapon::UNSInventorySlider_Weapon()
{
	bUseTotalCount = false;
	NumSlots = 5;
	bUseItemOnActivateSlot = true;
}

void UNSInventorySlider_Weapon::BeginPlay()
{
	Super::BeginPlay();

	EquipmentComponent = GetOwner()->FindComponentByClass<UNSEquipmentComponent>();
	if (!EquipmentComponent)
	{
		UE_LOG(LogInventorySlider, Display, TEXT("Equipment component not found in %s"), *GetOwner()->GetName())
	}
}

bool UNSInventorySlider_Weapon::IsItemMustBeAdd_Implementation(TSubclassOf<UNSItemDefinition> Item) const
{
	if (!Item) return false;

	const auto EquipmentFragment = GetDefault<UNSItemDefinition>(Item)->FindFragmentByClass<UFragment_Equipable>();
	if (!EquipmentFragment) return false;
	
	return EEquipmentType::Weapon == EquipmentFragment->GetDefinitionClass().GetDefaultObject()->Type;
}

void UNSInventorySlider_Weapon::UseItem_Implementation(const FSliderEntry& Item)
{
	if (EquipmentComponent && Item.AssignedInstance)
	{
		EquipmentComponent->EquipItem(Item.AssignedInstance);
	}
}
