
#if WITH_AUTOMATION_TESTS

#include "EquipmentTest.h"
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemInstance.h"
#include "Misc/AutomationTest.h"
#include "Equipment/NSEquipmentComponent.h"
#include "Utils/NSTestUtils.h"
#include "EngineUtils.h"
#include "Items/NSItemContainer.h"
#include "NSInventorySlider.h"

namespace
{
	FString WeaponContainerBP_1 = "Blueprint'/Game/NetworkShoter/Tests/Test_SwordContainer.Test_SwordContainer'";
	FString WeaponContainerBP_2 = "Blueprint'/Game/NetworkShoter/Tests/Test_RiffleContainer.Test_RiffleContainer'";
	
	bool SpawnItemContainerAndInteract(UWorld* World, APawn* Pawn, FString ContainerName)
	{
		const FTransform InitialTransform{FVector{1200.f}};	//or spawn prams always spawn
		const auto Container = NSTestUtils::SpawnBPActor<ANSItemContainer>(World, ContainerName, InitialTransform);
		if (!Container) return false;
		
		return Container->Execute_InteractWithPawn(Container, Pawn);
	}
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEquipmentReEquipTest, "NetworkShooter.Equipment.ReEquip", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)


bool FEquipmentReEquipTest::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");
	
	UWorld* World = NSTestUtils::GetTestWorld();
	if (!TestNotNull("World exist", World)) return false;
	
	const auto Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!TestNotNull("Pawn exist", Pawn)) return false;

	const auto Inventory = Pawn->FindComponentByClass<UNSInventoryComponent>();
	const auto Equipment = Pawn->FindComponentByClass<UNSEquipmentComponent>();
	if (!TestNotNull("Inventory exist", Inventory)) return false;
	if (!TestNotNull("EquipmentExist", Equipment)) return false;

	while (const auto Slider = Pawn->FindComponentByClass<UNSInventorySlider>())	//slider can equip first item automatic disable it
	{
		Slider->DestroyComponent();
	}
	
	//Add first weapon
	if (!SpawnItemContainerAndInteract(World, Pawn, WeaponContainerBP_1)) { return false; }
	if (!TestTrue("Item Added", Inventory->GetInventory().Num() == 1)) return false;
	const auto FirstWeaponItem = Inventory->GetInventory()[0].Item;
	
	Equipment->EquipItem(FirstWeaponItem);
	const auto FirstWeapon = Equipment->GetEquipmentBySlot(EEquipmentSlot::Weapon).EquipmentInstance;
	if (!TestNotNull("Weapon exist", FirstWeapon)) return false;
	if (!TestTrue("Inventory empty", Inventory->GetInventory().Num() == 0)) return false;

	
	//Add second weapon
	SpawnItemContainerAndInteract(World, Pawn, WeaponContainerBP_2);
	if (!TestTrueExpr(Inventory->GetInventory().Num() == 1)) return false;
	const auto SecondWeaponItem = Inventory->GetInventory()[0].Item;
	
	Equipment->EquipItem(SecondWeaponItem);
	const auto SecondWeapon = Equipment->GetEquipmentBySlot(EEquipmentSlot::Weapon).EquipmentInstance;

	if (!TestTrue("Weapon switched", FirstWeapon != SecondWeapon)) return false;;
	if (!TestTrue("First weapon returned in Inventory", Inventory->GetInventory()[0].Item == FirstWeaponItem)) return false;
	
	return true;
}


#endif
