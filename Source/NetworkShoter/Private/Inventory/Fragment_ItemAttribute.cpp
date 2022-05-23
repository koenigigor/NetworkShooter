// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Fragment_ItemAttribute.h"

#include "Inventory/NSItemInstance.h"

void UFragment_ItemAttribute::OnInstanceCreated(UNSItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);
	
	if (!Instance) return;
	
	for (const auto& Attribute : Attributes)
	{
		Instance->AddStatTagValue(Attribute.Key, Attribute.Value);
	}
}
