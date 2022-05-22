// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentLib.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None, //slot not specified (some consumable items)
	Weapon,
	Body
	//ring1, ring2 etc
};


UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	None,
	Weapon,
	Body
	//ring, TwoHandWeapon, etc
};

UCLASS()
class NETWORKSHOTER_API UNSEquipmentLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**	Array if item san be equipped in different slots (eq Ring -> ring1 or ring2) */
	UFUNCTION(BlueprintCallable, Category="Equipment")
	static TArray<EEquipmentSlot> ResolveEquipmentSlot(EEquipmentType Equipment);
};