// Fill out your copyright notice in the Description page of Project Settings.


#if WITH_AUTOMATION_WORKER

#include "Tests/InventoryTest.h"
#include "CoreMinimal.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/Fragment_ItemInfo.h"
#include "Inventory/NSItemDefinition.h"
#include "Inventory/NSItemInstance.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryDefaultsTest, "NetworkShooter.Inventory.Defaults", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryItemsTest, "NetworkShooter.Inventory.Items", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

UNSItemDefinitionTest::UNSItemDefinitionTest()
{
	auto ItemInfo = NewObject<UFragment_ItemInfo>();
	ItemInfo->bIsStackable = true;
	Fragments.Add(ItemInfo);
	
	UE_LOG(LogTemp, Display, TEXT("---Test Def construclor called---"))

}

bool FInventoryDefaultsTest::RunTest(const FString& Parameters)
{
	const int32 DefaultItemsCount = 0;
	
	//create inventory
	auto Inventory = NewObject<UNSInventoryComponent>();
	TestNotNull("Inventory exist", Inventory);

	TestTrue("DefaultItemCount", Inventory->GetInventory().Num() == DefaultItemsCount);
	
	return true;
}

bool FInventoryItemsTest::RunTest(const FString& Parameters)
{
	//create inventory
	auto Inventory = NewObject<UNSInventoryComponent>();
	TestNotNull("Inventory exist", Inventory);

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

	
	//removing
	
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


#endif

