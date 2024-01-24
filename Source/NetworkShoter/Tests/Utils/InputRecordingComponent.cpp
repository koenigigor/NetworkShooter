// Fill out your copyright notice in the Description page of Project Settings.


#include "InputRecordingComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedPlayerInput.h"
#include "JsonUtils.h"


UInputRecordingComponent::UInputRecordingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UInputRecordingComponent::BeginPlay()
{
	Super::BeginPlay();
	
	const auto Pawn = Cast<APawn>(GetOwner());
	ensure(Pawn);
	const auto PC = Pawn->GetController<APlayerController>();
	ensure(PC);

	PlayerInput = PC->PlayerInput;
	InputComponent = GetOwner()->InputComponent;

	EnhancedPlayerInput = Cast<UEnhancedPlayerInput>(PlayerInput);
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	ensure(PlayerInput);
	ensure(InputComponent);
	
	InputData.InitialTransform = GetOwner()->GetTransform();
}

void UInputRecordingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	JsonUtils::WriteInputData(GenerateFilename(), InputData);
}


void UInputRecordingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AppendBindings();
}

void UInputRecordingComponent::AppendBindings()
{
	auto& BindingData = InputData.BindingsData.AddDefaulted_GetRef();
	BindingData.Time = GetWorld()->GetTimeSeconds();

	//axis bindings
	for (const auto& AxisBinding : InputComponent->AxisBindings)
	{
		BindingData.AxisData.Add(FAxisData(AxisBinding.AxisName, AxisBinding.AxisValue));
	}

	//action bindings
	const auto NumActions = InputComponent->GetNumActionBindings();
	for (int32 Index = 0; Index < NumActions; ++Index)
	{
		const auto Action = InputComponent->GetActionBinding(Index);
		const auto ActionKeys = PlayerInput->GetKeysForAction(Action.GetActionName());
		ensure(ActionKeys.Num() > 0);
		const auto ActionKey = ActionKeys[0];

		const auto Pressed = ActionKeys.ContainsByPredicate([=](const FInputActionKeyMapping& Key)
		{
			return PlayerInput->IsPressed(Key.Key);
		});
		
		//make 1 record for action name, determine press/release by previous state
		const bool AlreadyContain = BindingData.ActionsData.ContainsByPredicate([=](const FActionsData& ActionsData){ return ActionsData.Key == ActionKey.Key; });
		if (!AlreadyContain)
		{
			BindingData.ActionsData.Add(FActionsData(Action.GetActionName(), ActionKey.Key, Pressed));
		} 
	}

	//enhanced actions
	for (const auto& ActionBinding : EnhancedInputComponent->GetActionEventBindings())
	{
		const auto Action = ActionBinding->GetAction();
		const auto ActionName = Action->GetName();

		const auto ActionValue = EnhancedPlayerInput->GetActionValue(Action);
		const auto Value = ActionValue.Get<FVector>();
		
		if (!BindingData.EnhancedActionsData.Contains(ActionName))
		{
			BindingData.EnhancedActionsData.Add(FEnhancedActionData(ActionName, Value));
		}
	}
}

FString UInputRecordingComponent::GenerateFilename() const
{
	const auto Folder = FPaths::ProjectSavedDir() + "TestData/";
	const auto Time = FDateTime::Now().ToString();
	return Folder + FileName + "_" + Time + ".Json";
}
