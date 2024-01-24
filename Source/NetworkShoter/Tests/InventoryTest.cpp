// Fill out your copyright notice in the Description page of Project Settings.


#if WITH_AUTOMATION_TESTS

#include "InventoryTest.h"
#include "CoreMinimal.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/Fragment_ItemInfo.h"
#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"
#include "Misc/AutomationTest.h"
#include "Items/NSItemContainer.h"
#include "Utils/NSTestUtils.h"
#include "Kismet/GameplayStatics.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryAddRemoveTest, "NetworkShooter.Inventory.AddAndRemoveDifferentWays",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryCountTest, "NetworkShooter.Inventory.CountCalculatesProperly",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventorySpawnActorTest, "NetworkShooter.Inventory.SpawnCollectableActorAndCollect",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

namespace
{
const FString InventoryTestLevel = "/Game/NetworkShoter/Tests/EmptyTestLevel";
const FString LandmineContainerBP = "Blueprint'/Game/NetworkShoter/Tests/Test_landmineContainer.Test_landmineContainer'";
}

UStackableDefinition::UStackableDefinition()
{
	auto ItemInfo = NewObject<UFragment_ItemInfo>();
	ItemInfo->bIsStackable = true;
	Fragments.Add(ItemInfo);
}

/** Add Remove items Different ways */
bool FInventoryAddRemoveTest::RunTest(const FString& Parameters)
{
	//create inventory
	const auto Inventory = NewObject<UNSInventoryComponent>();
	TestNotNull("Inventory exist", Inventory);

	AddInfo("Add items");

	Inventory->AddItem_Definition(UNSItemDefinition::StaticClass());
	TestTrue("Add first item", Inventory->GetInventory().Num() == 1);

	Inventory->AddItem_Definition(UStackableDefinition::StaticClass());
	TestTrue("Add second item", Inventory->GetInventory().Num() == 2);

	const auto ItemInst = GetDefault<UStackableDefinition>()->CreateInstance(Inventory);
	Inventory->AddItem_Instance(ItemInst, 2);
	TestTrue("Adds in stack", Inventory->GetInventory().Num() == 2);

	auto StackCount = Inventory->FindItem(UStackableDefinition::StaticClass()).StackCount;
	TestTrue("Check count in stack", StackCount == 3);

	AddInfo("Remove items");

	TArray<FInventoryEntry> RemovedItems;
	Inventory->RemoveItem(UStackableDefinition::StaticClass(), RemovedItems, 1, true);

	StackCount = Inventory->FindItem(UStackableDefinition::StaticClass()).StackCount;
	TestTrue("Remove 1 from stack", StackCount == 2);

	Inventory->RemoveItem(ItemInst, RemovedItems, 99999, true, false);
	TestTrue("remove instance who merged with previous stack, (nothing remove)", Inventory->GetInventory().Num() == 2);

	const auto StackableInstance = Inventory->FindItem(UStackableDefinition::StaticClass()).Item;
	Inventory->RemoveItem(StackableInstance, RemovedItems, 99999, true, false);
	TestTrue("remove 99999 from stack", Inventory->GetInventory().Num() == 1);

	Inventory->RemoveItem(UNSItemDefinition::StaticClass(), RemovedItems, 99999, true, false);
	TestTrue("remove last item", Inventory->GetInventory().Num() == 0);

	return true;
}

/** Count Stackable/nonStackable items */
bool FInventoryCountTest::RunTest(const FString& Parameters)
{
	const auto Inventory = NewObject<UNSInventoryComponent>();
	TestNotNull("Inventory exist", Inventory);

	AddInfo("Add stackable and non Stackable items");

	Inventory->AddItem_Definition(UStackableDefinition::StaticClass(), 2);
	Inventory->AddItem_Definition(UStackableDefinition::StaticClass(), 2);
	//1 stackable slot

	Inventory->AddItem_Definition(UNSItemDefinition::StaticClass(), 2);
	Inventory->AddItem_Definition(UNSItemDefinition::StaticClass());
	//3 non stackable items

	TestTrueExpr(Inventory->GetTotalCount(UStackableDefinition::StaticClass()) == 4);
	TestTrueExpr(Inventory->GetTotalCount(UNSItemDefinition::StaticClass()) == 3);
	TestTrueExpr(Inventory->GetInventory().Num() == 4);

	AddInfo("Remove stackable and non Stackable items");

	TArray<FInventoryEntry> RemovedItems;
	Inventory->RemoveItem(UStackableDefinition::StaticClass(), RemovedItems);
	Inventory->RemoveItem(UNSItemDefinition::StaticClass(), RemovedItems);

	TestTrueExpr(Inventory->GetTotalCount(UStackableDefinition::StaticClass()) == 3);
	TestTrueExpr(Inventory->GetTotalCount(UNSItemDefinition::StaticClass()) == 2);
	TestTrueExpr(Inventory->GetInventory().Num() == 3);

	return true;
}

/** Interact with item container */
bool FInventorySpawnActorTest::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel(InventoryTestLevel);

	UWorld* World = NSTestUtils::GetTestWorld();
	TestNotNull("World exist", World);

	const auto Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
	TestNotNull("Pawn exist", Pawn);

	const auto Inventory = Pawn->FindComponentByClass<UNSInventoryComponent>();
	TestNotNull("Inventory exist", Inventory);

	const FTransform InitialTransform{FVector{1000.f}}; //or spawn prams always spawn
	const auto Container = NSTestUtils::SpawnBPActor<ANSItemContainer>(World, LandmineContainerBP, InitialTransform);
	TestNotNull("Container spawned", Container);

	IInteractInterface::Execute_InteractWithPawn(Container, Pawn);
	TestTrue("Item added in inventory", Inventory->GetInventory().Num() == 1);

	return true;
}


#endif
