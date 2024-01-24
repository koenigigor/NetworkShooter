// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/NSInventorySlider.h"
#include "NSInventorySlider_Weapon.generated.h"

class UNSEquipmentComponent;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UNSInventorySlider_Weapon : public UNSInventorySlider
{
	GENERATED_BODY()
public:
	UNSInventorySlider_Weapon();
	virtual void BeginPlay() override;
	
protected:
	virtual bool IsItemMustBeAdd_Implementation(TSubclassOf<UNSItemDefinition> Item) const override;
	virtual void UseItem_Implementation(const FSliderEntry& Item) override;

	UPROPERTY()
	UNSEquipmentComponent* EquipmentComponent;
};
