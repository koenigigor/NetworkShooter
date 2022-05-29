

#if WITH_AUTOMATION_TESTS

#include "InventoryActorTest.h"
#include "CoreMinimal.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemInstance.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Items/NSItemContainer.h"
#include "NSTestUtils.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventorySpawnActorTest, "NetworkShooter.Inventory.SpawnCollectableActor", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

namespace
{
	constexpr char* LandmineContainerBP = "Blueprint'/Game/NetworkShoter/Actors/Containers/BP_LandmineContainer.BP_LandmineContainer'";
	
}

bool FInventorySpawnActorTest::RunTest(const FString& Parameters)
{
	NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = NSTestUtils::GetTestWorld();
	TestNotNull("World exist", World);
	
	const FTransform InitialTransform{FVector{1000.f}};	//or spawn prams always spawn
	auto Container = NSTestUtils::SpawnBPActor<ANSItemContainer>(World, FString(LandmineContainerBP), InitialTransform);
	TestNotNull("Container spawned", Container);
	
	
	//interact with item to add it in inventory
	
	
	return true;
}



#endif