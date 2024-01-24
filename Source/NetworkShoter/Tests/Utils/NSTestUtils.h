#pragma once

#if WITH_AUTOMATION_TESTS

#include "TestProxyActor.h"
#include "Tests/AutomationCommon.h"

class UInputAction;
class UInputComponent;

namespace NSTestUtils
{
	//spawnable actor for sent subclasses from editor
	const FString TestProxyActorBP = "Blueprint'/Game/NetworkShoter/Tests/TestProxyActor_BP.TestProxyActor_BP'";
	
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

	ATestProxyActor* SpawnTestProxyActor(UWorld* World);
	
	int32 GetActionMappingIndex(const UInputComponent* InputComponent, const FString& ActionName, EInputEvent InputEvent);
	int32 GetAxisMappingIndex(const UInputComponent* InputComponent, const FString& ActionName);
	// *Blueprint input add on next tick
	const UInputAction* FindEnhancedInputAction(const UEnhancedInputComponent* EnhancedInput, const FString& Name);

	/** update void function timeout seconds */
	class FNSUntilLatentCommand : public IAutomationLatentCommand
	{
	public:
		FNSUntilLatentCommand(TFunction<void()> InCallback, float InTimeout = 5.0f)
			: Callback(MoveTemp(InCallback))
			, Timeout(InTimeout)
		{}

		virtual bool Update() override;

	private:
		TFunction<void()> Callback;
		float Timeout;
	};

	FString GetTestDataDir();
}

#endif



