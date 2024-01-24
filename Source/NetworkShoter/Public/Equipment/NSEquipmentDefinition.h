// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentLib.h"
#include "NSEquipmentDefinition.generated.h"

class UNSEquipmentInstance;
class UNSEquipmentDefinition;

DEFINE_LOG_CATEGORY_STATIC(LogEFragment, All, All)


UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UNSEquipmentFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual void OnEquip(UNSEquipmentInstance* Instance){};
	virtual void OnUnequip(UNSEquipmentInstance* Instance){};
};

/**
 * 
 */
UCLASS(Blueprintable)
class NETWORKSHOTER_API UNSEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEquipmentType Type;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UNSEquipmentInstance> InstanceType;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced)
	TArray<TObjectPtr<UNSEquipmentFragment>> Fragments;

	TSubclassOf<UNSEquipmentInstance> GetInstanceType() const;
};
