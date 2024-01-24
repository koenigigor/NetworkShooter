// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputTrigger_MouseVisible.generated.h"

/** Trigger event if mouse is visible */
UCLASS(NotBlueprintable, meta = (DisplayName = "MouseVisible"))
class NETWORKSHOTER_API UInputTrigger_MouseVisible : public UInputTrigger
{
	GENERATED_BODY()
public:
	virtual ETriggerEventsSupported GetSupportedTriggerEvents() const override { return ETriggerEventsSupported::Instant; }

protected:
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;
};
