// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/NSItemDefinition.h"
#include "Fragment_UIInfo.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UFragment_UIInfo : public UNSInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name = "Item name placeholder";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* UIImage = nullptr;

	//description, etc
};
