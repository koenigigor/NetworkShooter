// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/NSEquipmentDefinition.h"

#include "Equipment/NSEquipmentInstance.h"

TSubclassOf<UNSEquipmentInstance> UNSEquipmentDefinition::GetInstanceType() const
{
	if (!InstanceType)
		return UNSEquipmentInstance::StaticClass();
	
	return InstanceType;
}
