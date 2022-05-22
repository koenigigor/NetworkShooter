// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Fragment_ItemAttribute.h"

#include "Inventory/NSItemInstance.h"

void UFragment_ItemAttribute::OnInstanceCreated(UNSItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	UE_LOG(LogTemp, Warning, TEXT("Attribute fragment called"))
	
	if (!Instance) return;

	UE_LOG(LogTemp, Warning, TEXT("Attribute fragment start loop"))
	
	for (const auto& Attribute : Attributes)
	{
		Instance->AddStatTagValue(Attribute.Key, Attribute.Value);
		UE_LOG(LogTemp, Warning, TEXT("Attribute fragment added %s, %f"), *Attribute.Key.ToString(), Attribute.Value)
	}
}
