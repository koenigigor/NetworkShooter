



#if WITH_AUTOMATION_TESTS

#include "CharacterInputTest.h"
#include "NSTestUtils.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "InputData.h"
#include "Utils/JsonUtils.h"


DEFINE_LOG_CATEGORY_STATIC(LogCharacterInputTest, All, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaracterJumpTest, "NetworkShooter.Character.Learn.Jump", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaracterWalkForwardTest, "NetworkShooter.Character.Learn.WalkForward", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaracterRunRecordingInputTest, "NetworkShooter.Character.Learn.RunRecordingInput", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter)


DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FJumpLatentCommand, UInputComponent*, InputComponent);
bool FJumpLatentCommand::Update()
{
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

class FPlaySavedInputDataLatentCommand : public IAutomationLatentCommand
{
public:
	FPlaySavedInputDataLatentCommand(UWorld* InWorld, UInputComponent* InInputComponent, TArray<FBindingsData> InBindingData)
			:World(InWorld), InputComponent(InInputComponent), BindingsData(InBindingData)
	{
		if (World)
		{
			WorldStartTime = World->TimeSeconds;
		}
	};
	
	virtual bool Update() override;
private:
	UWorld* World = nullptr;
	UInputComponent* InputComponent = nullptr;
	TArray<FBindingsData> BindingsData;
	int32 Index = 0;
	float WorldStartTime = 0;
}; 
bool FPlaySavedInputDataLatentCommand::Update()
{
	if (!World && !InputComponent && BindingsData.Num()==0) return true;
	
	while (World->TimeSeconds - WorldStartTime >= BindingsData[Index].Time)
	{
		for (const auto& AxisData : BindingsData[Index].AxisData)
		{
			const auto AxisIndex = NSTestUtils::GetAxisMappingIndex(InputComponent, AxisData.Name.ToString());
			if (AxisIndex != INDEX_NONE)
			{
				InputComponent->AxisBindings[AxisIndex].AxisDelegate.Execute(AxisData.Value);
			}
		} 
		
		if (Index > 0)
		{
			for (int32 ActionIndex = 0; ActionIndex< BindingsData[Index].ActionsData.Num(); ++ActionIndex)
			{
				const auto& ActionData = BindingsData[Index].ActionsData[ActionIndex];
				const auto& PreviousState = BindingsData[Index-1].ActionsData[ActionIndex].State;

				if (ActionData.State != PreviousState)
				{
					const auto MappingIndex = NSTestUtils::GetActionMappingIndex(InputComponent, ActionData.Name.ToString(), ActionData.State ? IE_Pressed : IE_Released);
					if (MappingIndex != INDEX_NONE)
					{
						InputComponent->GetActionBinding(MappingIndex).ActionDelegate.Execute(ActionData.Key);
					}
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
	TestNotNull("World exist", World);

	const auto Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	TestNotNull("Character valid", Character);

	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(5.f));
	ADD_LATENT_AUTOMATION_COMMAND(FJumpLatentCommand(Character->InputComponent));
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
		[=]() //& invalid pointer?
		{
			if (MoveForwardIndex == INDEX_NONE)	return;
			Character->InputComponent->AxisBindings[MoveForwardIndex].AxisDelegate.Execute(1.f);		
		},
		4.f));
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(3.f));

	return true;
}

bool FCaracterRunRecordingInputTest::RunTest(const FString& Parameters)
{
	const auto Level = NSTestUtils::RunLevel("/Game/NetworkShoter/Tests/EmptyTestLevel");

	UWorld* World = NSTestUtils::GetTestWorld();
	TestNotNull("World exist", World);

	const auto Character = UGameplayStatics::GetPlayerCharacter(World, 0);
	TestNotNull("Character valid", Character);
	TestNotNull("Controller valid", Character->GetController());

	const FString InputDataFileName = NSTestUtils::GetTestDataDir().Append("CharacterInputRecord.Json");
	FInputData InputData;
	JsonUtils::ReadInputData(InputDataFileName, InputData);

	Character->SetActorTransform(InputData.InitialTransform);
	Character->GetController()->SetControlRotation(InputData.InitialTransform.Rotator());

	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
	ADD_LATENT_AUTOMATION_COMMAND(FPlaySavedInputDataLatentCommand(World, Character->InputComponent, InputData.BindingsData));
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.f));
	return true;
}


#endif