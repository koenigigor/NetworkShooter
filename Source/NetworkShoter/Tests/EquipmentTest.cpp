
#if WITH_AUTOMATION_TESTS

#include "EquipmentTest.h"
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemInstance.h"
#include "Misc/AutomationTest.h"
#include "Equipment/NSEquipmentComponent.h"
#include "NSTestUtils.h"
#include "EngineUtils.h"
#include "Items/NSItemContainer.h"



namespace
{
	void SpawnItemContainerAndInteract(UWorld* World, APawn* Pawn, FString ContainerName)
	{
		const FTransform InitialTransform{FVector{1200.f}};	//or spawn prams always spawn
		auto Container = NSTestUtils::SpawnBPActor<ANSItemContainer>(World, FString(ContainerName), InitialTransform);
	
		//interact with item to add it in inventory
		Cast<IInteractInterface>(Container)->Execute_InteractWithPawn(Container, Pawn);
	}
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEquipmentReEquipTest, "NetworkShooter.Equipment.ReEquip", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)


bool FEquipmentReEquipTest::RunTest(const FString& Parameters)
{
	NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");
	
	UWorld* World = NSTestUtils::GetTestWorld();
	TestNotNull("World exist", World);
	
	auto Pawn = NSTestUtils::SpawnBPActor<APawn>(World, "Blueprint'/Game/NetworkShoter/Character/ShoterPlayer_BP.ShoterPlayer_BP'");
	TestNotNull("Pawn exist", Pawn);
	
	auto Inventory = Pawn->FindComponentByClass<UNSInventoryComponent>();
	auto Equipment = Pawn->FindComponentByClass<UNSEquipmentComponent>();
	TestNotNull("Inventory exist", Inventory);
	TestNotNull("EquipmentExist", Equipment);
	
	//FString WeaponDefBP_1 = "Blueprint'/Game/NetworkShoter/Items/ItemSword.ItemSword'";
	//FString WeaponDefBP_2 = "Blueprint'/Game/NetworkShoter/Items/ItemRiffle.ItemRiffle'";

	FString WeaponContainerBP_1 = "Blueprint'/Game/NetworkShoter/Actors/Containers/BP_SwordContainer.BP_SwordContainer'";
	FString WeaponContainerBP_2 = "Blueprint'/Game/NetworkShoter/Actors/Containers/BP_RiffleContainer.BP_RiffleContainer'";
	
	SpawnItemContainerAndInteract(World, Pawn, WeaponContainerBP_1);

	TestTrueExpr(Inventory->GetInventory().Num() == 1);
	auto FirstWeaponItem = Inventory->GetInventory()[0].Item;
	TArray<FInventoryEntry> RemovedItems;
	Inventory->RemoveItem(FirstWeaponItem, RemovedItems);
	Equipment->EquipItem(FirstWeaponItem);
	auto FirstWeapon = Equipment->GetEquipmentBySlot(EEquipmentSlot::Weapon).EquipmentInstance;
	TestNotNull("Weapon exist", FirstWeapon);

	TestTrue("Inventory must be empty", Inventory->GetInventory().Num() == 0);

	SpawnItemContainerAndInteract(World, Pawn, WeaponContainerBP_2); //Add second weapon
	auto SecondWeaponItem = Inventory->GetInventory()[0].Item;
	Inventory->RemoveItem(SecondWeaponItem, RemovedItems);
	Equipment->EquipItem(FirstWeaponItem);
	auto SecondWeapon = Equipment->GetEquipmentBySlot(EEquipmentSlot::Weapon).EquipmentInstance;

	TestTrue("Weapon switched", FirstWeapon != SecondWeapon);
	TestTrue("First weapon returned in Inventory", Inventory->GetInventory()[0].Item == FirstWeaponItem);
	
	return true;
}
//todo item in inventory, if equip remove it


#endif
