// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/NSItemDefinition.h"
#include "Fragment_Equipable.generated.h"

class UNSEquipmentDefinition;

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UFragment_Equipable : public UNSInventoryItemFragment
{
	GENERATED_BODY()
public:
	TSubclassOf<UNSEquipmentDefinition> GetDefinitionClass() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UNSEquipmentDefinition> EquipmentDefinition;
};
