// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/NSItemSlider.h"
#include "NSWeaponSlider.generated.h"

class UNSEquipmentComponent;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UNSWeaponSlider : public UNSItemSlider
{
	GENERATED_BODY()
	
	UNSWeaponSlider();
	virtual void BeginPlay() override;
	virtual bool IsItemMustBeAddedOnSlider_Implementation(UNSItemInstance* Item) override;
	virtual void OnSlotSelected_Implementation(int32 PreviousSlot) override;

	UPROPERTY()
	UNSEquipmentComponent* Equipment = nullptr;
};
