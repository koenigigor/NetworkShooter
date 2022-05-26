

#if WITH_AUTOMATION_WORKER

#include "InventoryActorTest.h"
#include "CoreMinimal.h"
#include "Inventory/NSInventoryComponent.h"
#include "Inventory/NSItemInstance.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Items/NSItemContainer.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventorySpawnActorTest, "NetworkShooter.Inventory.SpawnCollectableActor", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

namespace
{
	constexpr char* LandmineContainerBP = "Blueprint'/Game/NetworkShoter/Actors/Containers/BP_LandmineContainer.BP_LandmineContainer'";
	
	UWorld* GetTestGameWorld()
	{
		const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
		for ( const FWorldContext& Context : WorldContexts )
		{
			if ( (  Context.WorldType == EWorldType::PIE  || Context.WorldType == EWorldType::Game ) && Context.World() )
			{
				return Context.World();
			}
		}
	
		return nullptr;
	}
	
	
	class LevelScope
	{
	public:
		LevelScope(){};
		LevelScope(const FString& MapName) { AutomationOpenMap(MapName); };
		~LevelScope() { ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand); };
	};
}

bool FInventorySpawnActorTest::RunTest(const FString& Parameters)
{
	LevelScope("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = GetTestGameWorld();
	TestNotNull("World exist", World);
	
	//BP ItemToPick
   	const UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *FString(LandmineContainerBP));
   	TestNotNull("Blueprint exist", Blueprint);

	//spawn ItemToPick
	const FTransform InitialTransform{FVector{1000.f}};	//or spawn prams always spawn
	auto Container = World->SpawnActor<ANSItemContainer>(Blueprint->GeneratedClass, InitialTransform);
	TestNotNull("Container spawned", Container);
	
	
	//interact with item to add it in inventory
	
	
	return true;
}



#endif