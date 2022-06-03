// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NSGameplayAbility.generated.h"

UENUM()
enum class EActivationPolicy : uint8
{
	InputPress,
	InputHold
};

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UNSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	EActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	UPROPERTY(EditDefaultsOnly)
	EActivationPolicy ActivationPolicy;
};
