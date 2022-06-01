// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/NSItemDefinition.h"
#include "GameplayTagContainer.h"
#include "Fragment_ItemAttribute.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UFragment_ItemAttribute : public UNSInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UNSItemInstance* Instance) override;

	UPROPERTY(EditDefaultsOnly, meta=(Categories="ItemAttribute"))
	TMap<FGameplayTag, float> Attributes;
};
