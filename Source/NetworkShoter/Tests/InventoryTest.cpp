// Fill out your copyright notice in the Description page of Project Settings.


#if WITH_AUTOMATION_WORKER

#include "Tests/InventoryTest.h"
#include "CoreMinimal.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/Fragment_ItemInfo.h"
#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryAddRemoveTest, "NetworkShooter.Inventory.AddAndRemove", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryCountTest, "NetworkShooter.Inventory.Count", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)



UNSItemDefinitionTest::UNSItemDefinitionTest()
{
	auto ItemInfo = NewObject<UFragment_ItemInfo>();
	ItemInfo->bIsStackable = true;
	Fragments.Add(ItemInfo);
}

/** Add Remove items Different ways */
bool FInventoryAddRemoveTest::RunTest(const FString& Parameters)
{
	//create inventory
	auto Inventory = NewObject<UNSInventoryComponent>();
	TestNotNull("Inventory exist", Inventory);

	AddInfo("Add items");

	Inventory->AddItem_Definition(UNSItemDefinition::StaticClass());
	TestTrue("Add first item", Inventory->GetInventory().Num() == 1);
	
	Inventory->AddItem_Definition(UNSItemDefinitionTest::StaticClass());
	TestTrue("Add second item", Inventory->GetInventory().Num() == 2);
	
	
	auto Item = NewObject<UNSItemInstance>(Inventory, UNSItemInstance::StaticClass());
	Item->InitDefinition(UNSItemDefinitionTest::StaticClass());
	Inventory->AddItem_Instance(Item, 2);

	TestTrue("Add stackable item, count not change", Inventory->GetInventory().Num() == 2);

	auto Stack = Inventory->FindItem(UNSItemDefinitionTest::StaticClass()).StackCount;
	TestTrue("Check count in stack", Stack == 3);

	
	AddInfo("Removing items");
	
	TArray<FInventoryEntry> RemovedItems;
	Inventory->RemoveItem(UNSItemDefinitionTest::StaticClass(), RemovedItems, 1, true);
	
	Stack = Inventory->FindItem(UNSItemDefinitionTest::StaticClass()).StackCount;
	TestTrue("Remove 1 from stack", Stack == 2);

	
	Inventory->RemoveItem(Item, RemovedItems, 99999, true, false);
	TestTrue("remove 99999 from stack", Inventory->GetInventory().Num() == 1);

	Inventory->RemoveItem(UNSItemDefinition::StaticClass(), RemovedItems, 99999, true, false);
	TestTrue("remove last item", Inventory->GetInventory().Num() == 0);
	
	
	return true;
}

/** Count Stackable/nonStackable items */
bool FInventoryCountTest::RunTest(const FString& Parameters)
{
	auto Inventory = NewObject<UNSInventoryComponent>();
	TestNotNull("Inventory exist", Inventory);
	TArray<FInventoryEntry> RemovedItems;

	AddInfo("Add stackable and non Stackable items");
	
	Inventory->AddItem_Definition(UNSItemDefinitionTest::StaticClass(), 2);
	Inventory->AddItem_Definition(UNSItemDefinitionTest::StaticClass(), 2);
	//1 stackable slot

	Inventory->AddItem_Definition(UNSItemDefinition::StaticClass(), 2);
	Inventory->AddItem_Definition(UNSItemDefinition::StaticClass());
	//3 non stackable items

	TestTrueExpr(Inventory->GetTotalCount(UNSItemDefinitionTest::StaticClass()) == 4);
	TestTrueExpr(Inventory->GetTotalCount(UNSItemDefinition::StaticClass()) == 3);	
	TestTrueExpr(Inventory->GetInventory().Num() == 4);

	AddInfo("Remove stackable and non Stackable items");
	
	Inventory->RemoveItem(UNSItemDefinitionTest::StaticClass(), RemovedItems);
	Inventory->RemoveItem(UNSItemDefinition::StaticClass(), RemovedItems);

	TestTrueExpr(Inventory->GetTotalCount(UNSItemDefinitionTest::StaticClass()) == 3);
	TestTrueExpr(Inventory->GetTotalCount(UNSItemDefinition::StaticClass()) == 2);
	TestTrueExpr(Inventory->GetInventory().Num() == 3);
	
	return true;
}


#endif

