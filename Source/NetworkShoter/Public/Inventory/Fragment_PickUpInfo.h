// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/NSItemDefinition.h"
#include "Fragment_PickUpInfo.generated.h"

/**
 * info about pickup item
 * for setup container who keep item:
 * - mesh for visual representation
 * - widget who popup around container (?)
 * - etc
 */
UCLASS()
class NETWORKSHOTER_API UFragment_PickUpInfo : public UNSInventoryItemFragment
{
	GENERATED_BODY()

public:
	/** Weapon mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* Mesh = nullptr;
};
