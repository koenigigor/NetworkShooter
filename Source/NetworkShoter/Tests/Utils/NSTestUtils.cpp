#if WITH_AUTOMATION_TESTS

#include "NSTestUtils.h"
#include "EnhancedInputComponent.h"


UWorld* NSTestUtils::GetTestWorld()
{
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
}

ATestProxyActor* NSTestUtils::SpawnTestProxyActor(UWorld* World)
{
	return SpawnBPActor<ATestProxyActor>(World, TestProxyActorBP);
}

int32 NSTestUtils::GetActionMappingIndex(const UInputComponent* InputComponent, const FString& ActionName,
                                         EInputEvent InputEvent)
{
	if (!InputComponent) return INDEX_NONE;
	
	const auto NumBindings = InputComponent->GetNumActionBindings();
	for (int32 i = 0; i != NumBindings; ++i)
	{
		const auto Action = InputComponent->GetActionBinding(i);
		if (Action.GetActionName().ToString().Equals(ActionName) && Action.KeyEvent==InputEvent)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

int32 NSTestUtils::GetAxisMappingIndex(const UInputComponent* InputComponent, const FString& ActionName)
{
	if (!InputComponent) return INDEX_NONE;
	
	return InputComponent->AxisBindings.IndexOfByPredicate([&](const FInputAxisBinding& Binding)
	{
		return Binding.AxisName.ToString().Equals(ActionName);
	});
}

const UInputAction* NSTestUtils::FindEnhancedInputAction(const UEnhancedInputComponent* EnhancedInput, const FString& Name)
{
	const int32 ActionIndex = EnhancedInput->GetActionEventBindings().IndexOfByPredicate([=](const TUniquePtr<FEnhancedInputActionEventBinding>& ActionBinding)
	{
		return ActionBinding->GetAction()->GetName().Equals(Name);
	});

	if (ActionIndex == INDEX_NONE) return nullptr;
	
	return EnhancedInput->GetActionEventBindings()[ActionIndex].Get()->GetAction();		
}

bool NSTestUtils::FNSUntilLatentCommand::Update()
{
	const double NewTime = FPlatformTime::Seconds();
	if ( NewTime - StartTime >= Timeout )
	{
		return true;
	}
	
	Callback();

	return false;
}

FString NSTestUtils::GetTestDataDir()
{
	return FPaths::GameSourceDir().Append("NetworkShoter/Tests/Data/");
}

#endif
