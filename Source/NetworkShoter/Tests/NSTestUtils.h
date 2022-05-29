#pragma once

#if WITH_AUTOMATION_TESTS


#include "Tests/AutomationCommon.h"

namespace NSTestUtils
{
	
	class RunLevel
	{
	public:
		RunLevel(const FString& MapName) { AutomationOpenMap(MapName); };
		~RunLevel() { ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand); };
	};

	/** GetTestWorld() == GetWorld() */
	UWorld* GetTestWorld();

	template<typename T>
	T* SpawnBPActor(UWorld* World, const FString& Name, FTransform Transform = FTransform::Identity)
	{
		const UBlueprint* Blueprint = LoadObject<UBlueprint>(nullptr, *Name);
		return (World && Blueprint) ? (T*)World->SpawnActor<T>(Blueprint->GeneratedClass, Transform) : nullptr;
	}
	
}

#endif



