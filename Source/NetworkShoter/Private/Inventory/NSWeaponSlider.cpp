// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/NSWeaponSlider.h"

#include "Equipment/NSEquipmentComponent.h"
#include "Equipment/NSEquipmentDefinition.h"
#include "Inventory/Fragment_Equipable.h"
#include "Inventory/NSItemInstance.h"

UNSWeaponSlider::UNSWeaponSlider()
{
	SliderType = ESliderType::ByItemInstance;
	MaxSlots = 5;
	bChangeActiveSlotOnInvalidate=false;
	bCacheActiveRemovedSlot=true;
}

void UNSWeaponSlider::BeginPlay()
{
	Super::BeginPlay();

	Equipment = GetOwner()->FindComponentByClass<UNSEquipmentComponent>();
	ensure(Equipment);
}

bool UNSWeaponSlider::IsItemMustBeAddedOnSlider_Implementation(UNSItemInstance* Item)
{
	if (!Item) return false;
	const auto EquipableFragment = Item->FindFragmentByClass<UFragment_Equipable>();
	if (!EquipableFragment) return false;
	
	if (EEquipmentType::Weapon == EquipableFragment->GetDefinitionClass().GetDefaultObject()->Type) return true;

	return false;
}

void UNSWeaponSlider::OnSlotSelected_Implementation(int32 PreviousSlot)
{
	Super::OnSlotSelected_Implementation(PreviousSlot);

	Equipment->EquipItem(GetSelectedInstance());
}
