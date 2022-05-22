// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Fragment_Equipable.h"

#include "Equipment/NSEquipmentDefinition.h"

TSubclassOf<UNSEquipmentDefinition> UFragment_Equipable::GetDefinitionClass() const
{
	if (!EquipmentDefinition)
		return UNSEquipmentDefinition::StaticClass();

	return EquipmentDefinition;
}
