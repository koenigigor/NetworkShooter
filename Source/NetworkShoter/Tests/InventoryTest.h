// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Inventory/NSItemDefinition.h"
#include "InventoryTest.generated.h"

/*
 *	Test definition with enable stack
 *	//todo how exclude it from package?
 */
UCLASS()
class UNSItemDefinitionTest : public UNSItemDefinition
{
	GENERATED_BODY()
	
public:	
	UNSItemDefinitionTest();

};
