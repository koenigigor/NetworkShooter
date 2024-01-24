#if WITH_AUTOMATION_TESTS

#include "CharacterInputTest.h"
#include "Utils/NSTestUtils.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/InputData.h"
#include "Utils/JsonUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"


DEFINE_LOG_CATEGORY_STATIC(LogCharacterInputTest, All, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaracterJumpTest, "NetworkShooter.Character.Learn.Jump",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaracterWalkForwardTest, "NetworkShooter.Character.Learn.WalkForward",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaracterEnhancedWalkForwardTest, "NetworkShooter.Character.Learn.EnhancedWalkForward",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaracterRunRecordingInputTest, "NetworkShooter.Character.Learn.RunRecordingInput",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)


DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FJumpLatentCommand, UInputComponent*, InputComponent);

bool FJumpLatentCommand::Update()
{
	if (!InputComponent) return true;

	const auto JumpIndex = NSTestUtils::GetActionMappingIndex(InputComponent, "Jump", IE_Pressed);
	if (JumpIndex == INDEX_NONE)
	{
		UE_LOG(LogCharacterInputTest, Warning, TEXT("Action Jump Not Found"))
		return true;
	}

	const auto& Action = InputComponent->GetActionBinding(JumpIndex);
	Action.ActionDelegate.Execute(EKeys::SpaceBar);

	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FEnhancedJumpLatentCommand, UInputComponent*, InputComponent);

bool FEnhancedJumpLatentCommand::Update()
{
	const auto EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput) return true;

	const auto ActionIndex = EnhancedInput->GetActionEventBindings().IndexOfByPredicate([](const TUniquePtr<FEnhancedInputActionEventBinding>& ActionBinding)
	{
		return ActionBinding->GetTriggerEvent() == ETriggerEvent::Started && ActionBinding->GetAction()->GetName().Equals("IA_Jump");
	});

	if (ActionIndex != INDEX_NONE)
	{
		EnhancedInput->GetActionEventBindings()[ActionIndex]->Execute(FInputActionInstance());
	}

	return true;
}


class FPlaySavedInputDataLatentCommand : public IAutomationLatentCommand
{
public:
	FPlaySavedInputDataLatentCommand(UWorld* InWorld, UInputComponent* InInputComponent, TArray<FBindingsData> InBindingData,
		UEnhancedPlayerInput* InEnhancedPlayerInput = nullptr)
		: World(InWorld), InputComponent(InInputComponent), BindingsData(InBindingData), EnhancedPlayerInput(InEnhancedPlayerInput)
	{
		if (World)
		{
			WorldStartTime = World->TimeSeconds;
			EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
		}
	};

	virtual bool Update() override;
private:
	UWorld* World = nullptr;
	UInputComponent* InputComponent = nullptr;
	UEnhancedInputComponent* EnhancedInput = nullptr;
	TArray<FBindingsData> BindingsData;
	UEnhancedPlayerInput* EnhancedPlayerInput = nullptr;
	int32 Index = 0;
	float WorldStartTime = 0;
};

bool FPlaySavedInputDataLatentCommand::Update()
{
	if (!World && !InputComponent && BindingsData.Num() == 0) return true;

	while (World->TimeSeconds - WorldStartTime >= BindingsData[Index].Time)
	{
		//trigger axis input
		for (const auto& AxisData : BindingsData[Index].AxisData)
		{
			const auto AxisIndex = NSTestUtils::GetAxisMappingIndex(InputComponent, AxisData.Name.ToString());
			if (AxisIndex != INDEX_NONE)
			{
				InputComponent->AxisBindings[AxisIndex].AxisDelegate.Execute(AxisData.Value);
			}
		}

		//trigger action input
		if (Index > 0)
		{
			for (int32 ActionIndex = 0; ActionIndex < BindingsData[Index].ActionsData.Num(); ++ActionIndex)
			{
				const auto& ActionData = BindingsData[Index].ActionsData[ActionIndex];
				const auto& PreviousState = BindingsData[Index - 1].ActionsData[ActionIndex].State;

				if (ActionData.State != PreviousState)
				{
					const auto MappingIndex = NSTestUtils::GetActionMappingIndex(InputComponent, ActionData.Name.ToString(),
						ActionData.State ? IE_Pressed : IE_Released);
					if (MappingIndex != INDEX_NONE)
					{
						InputComponent->GetActionBinding(MappingIndex).ActionDelegate.Execute(ActionData.Key);
					}
				}
			}
		}

		//trigger enhanced actions input
		if (EnhancedInput && EnhancedPlayerInput)
		{
			for (int32 ActionIndex = 0; ActionIndex < BindingsData[Index].EnhancedActionsData.Num(); ++ActionIndex)
			{
				const auto ActionData = BindingsData[Index].EnhancedActionsData[ActionIndex];
				const auto Action = NSTestUtils::FindEnhancedInputAction(EnhancedInput, ActionData.ActionName);
				if (Action)
				{
					EnhancedPlayerInput->InjectInputForAction(Action, FInputActionValue(ActionData.Value));
				}
			}
		}

		++Index;
		if (Index >= BindingsData.Num()) return true;
	}

	return false;
}


bool FCaracterJumpTest::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = NSTestUtils::GetTestWorld();
	if (!TestNotNull("World exist", World)) return false;

	const auto Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!TestNotNull("Character valid", Character)) return false;
	if (!TestNotNull("InputComponent valid", Character->InputComponent.Get())) return false;

	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(5.f));
	//ADD_LATENT_AUTOMATION_COMMAND(FJumpLatentCommand(Character->InputComponent));
	ADD_LATENT_AUTOMATION_COMMAND(FEnhancedJumpLatentCommand(Character->InputComponent))
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(5.f));

	return true;
}

bool FCaracterWalkForwardTest::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = NSTestUtils::GetTestWorld();
	TestNotNull("World exist", World);

	const auto Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	TestNotNull("Character valid", Character);

	const int32 MoveForwardIndex = NSTestUtils::GetAxisMappingIndex(Character->InputComponent, "MoveForward");

	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(3.f));
	ADD_LATENT_AUTOMATION_COMMAND(NSTestUtils::FNSUntilLatentCommand(
		[=]()
		{
		if (MoveForwardIndex == INDEX_NONE) return;
		Character->InputComponent->AxisBindings[MoveForwardIndex].AxisDelegate.Execute(1.f);
		},
		4.f));
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(3.f));

	return true;
}

bool FCaracterEnhancedWalkForwardTest::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = NSTestUtils::GetTestWorld();
	if (!TestNotNull("World exist", World)) return false;

	const auto Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!TestNotNull("Character valid", Character)) return false;

	const auto Controller = UGameplayStatics::GetPlayerController(World, 0);
	if (!TestNotNull("Controller valid", Controller)) return false;

	const auto EnhancedInput = Cast<UEnhancedInputComponent>(Character->InputComponent);
	if (!TestNotNull("Enhanced Input", EnhancedInput)) return false;

	const auto EnhancedPlayerInput = Cast<UEnhancedPlayerInput>(Controller->PlayerInput);
	if (!TestNotNull("EnhancedPlayerInput", EnhancedPlayerInput)) return false;

	const int32 MoveForwardIndex = EnhancedInput->GetActionEventBindings().IndexOfByPredicate(
		[](const TUniquePtr<FEnhancedInputActionEventBinding>& ActionBinding)
		{
			return ActionBinding->GetTriggerEvent() == ETriggerEvent::Triggered && ActionBinding->GetAction()->GetName().Equals("IA_Move");
		});

	if (!TestTrue("Found move forward InputAction", MoveForwardIndex != INDEX_NONE)) return false;

	const auto MoveForward = EnhancedInput->GetActionEventBindings()[MoveForwardIndex].Get();

	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(3.f));
	ADD_LATENT_AUTOMATION_COMMAND(NSTestUtils::FNSUntilLatentCommand(
		[=]()
		{
		EnhancedPlayerInput->InjectInputForAction(MoveForward->GetAction(), FInputActionValue(1.f) );
		},
		4.f));
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(3.f));

	return true;
}


bool FCaracterRunRecordingInputTest::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = NSTestUtils::GetTestWorld();
	if (!TestNotNull("World exist", World)) return false;

	const auto Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!TestNotNull("Character valid", Character)) return false;

	const auto Controller = Character->GetController<APlayerController>();
	if (!TestNotNull("Controller valid", Controller)) return false;

	const auto EnhancedPlayerInput = Cast<UEnhancedPlayerInput>(Controller->PlayerInput);

	const FString InputDataFileName = NSTestUtils::GetTestDataDir().Append("CharacterInputRecord.Json");
	FInputData InputData;
	JsonUtils::ReadInputData(InputDataFileName, InputData);

	Character->SetActorTransform(InputData.InitialTransform);
	Character->GetController()->SetControlRotation(InputData.InitialTransform.Rotator());

	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
	ADD_LATENT_AUTOMATION_COMMAND(FPlaySavedInputDataLatentCommand(World, Character->InputComponent, InputData.BindingsData, EnhancedPlayerInput));
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
	return true;
}


#endif
