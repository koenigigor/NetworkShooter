// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/InputTrigger_MouseVisible.h"

#include "EnhancedPlayerInput.h"

ETriggerState UInputTrigger_MouseVisible::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	return PlayerInput->GetOuterAPlayerController()->bShowMouseCursor ? ETriggerState::Triggered : ETriggerState::None;
}
