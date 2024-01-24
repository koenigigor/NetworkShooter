// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentLib.h"


TArray<EEquipmentSlot> UNSEquipmentLibrary::ResolveEquipmentSlot(EEquipmentType Equipment)
{
	TMap<EEquipmentType, TArray<EEquipmentSlot>> EquipmentAttitude
	{
		{EEquipmentType::None, {EEquipmentSlot::None}},
		{EEquipmentType::Weapon, {EEquipmentSlot::Weapon}},
		{EEquipmentType::Body,{EEquipmentSlot::Body}}
	};
	
	if (!EquipmentAttitude.Contains(Equipment))
	{
		ensureMsgf(false, TEXT("Attitude not found"));
	}

	return EquipmentAttitude[Equipment];
}	//TODO scheme like team attitude
