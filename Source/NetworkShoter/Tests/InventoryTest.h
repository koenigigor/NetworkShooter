// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Inventory/NSItemDefinition.h"
#include "InventoryTest.generated.h"

/*
 *	Testable definition for stackable item,
 *	cant create not uclass child, using fragments assumes GetDefaultObject, who not works without uclass macro 
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class UStackableDefinition : public UNSItemDefinition
{
	GENERATED_BODY()
	
public:	
	UStackableDefinition();

};
