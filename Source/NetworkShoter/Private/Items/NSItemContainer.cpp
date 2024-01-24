// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/NSItemContainer.h"

#include "Inventory/NSInventoryComponent.h"


ANSItemContainer::ANSItemContainer()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

bool ANSItemContainer::InteractWithPawn_Implementation(APawn* InteractWith)
{
	if (!InteractWith || !CanInteract()) return false;
	const auto Inventory = InteractWith->FindComponentByClass<UNSInventoryComponent>();
	if (!Inventory) return false;

	Inventory->AddItem_Definition(ItemToPick, Count);

	ItemGathered();
	return true;
}

bool ANSItemContainer::CanInteract_Implementation()
{
	return true;
}

void ANSItemContainer::ItemGathered_Implementation()
{
	Destroy();
}


