// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/NSItemDefinition.h"
#include "Fragment_ItemInfo.generated.h"

/**
 * this fragment keep info about item,
 * stackable, mass, inventory size etc.
 */
UCLASS()
class NETWORKSHOTER_API UFragment_ItemInfo : public UNSInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	bool bIsStackable = false;
};
