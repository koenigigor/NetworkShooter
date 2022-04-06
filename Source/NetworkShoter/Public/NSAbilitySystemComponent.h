// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NSAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputRelease, const FGameplayTagContainer&, TagContainer);

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UNSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	/** Send input release signal to ability (AbilityTask_WaitInputRelease_NS) */
	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SendInputReleaseToAbility(const FGameplayTagContainer& GameplayTagContainer);

	FInputRelease InputRelease;
};
