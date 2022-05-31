// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Equipment/NSEquipmentDefinition.h"
#include "EFragment_Attributes.generated.h"

class UEquipmentAttributes;




/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UEFragment_Attributes : public UNSEquipmentFragment
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEquipmentAttributes> EquipmentAttributes;
	
	virtual void OnEquip(UNSEquipmentInstance* Instance) override;
	virtual void OnUnequip(UNSEquipmentInstance* Instance) override;
};
