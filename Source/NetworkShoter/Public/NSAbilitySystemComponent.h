// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NSAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputRelease, const FGameplayTagContainer&, TagContainer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputPress, const FGameplayTagContainer&, TagContainer);

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UNSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	/** Send input release event to ability (AbilityTask_WaitInputRelease_NS) */
	UFUNCTION(BlueprintCallable)
	void SendInputReleaseToAbility(const FGameplayTagContainer& GameplayTagContainer);

	/** Send input press event to ability (AbilityTask_WaitInputPress_NS) */
	UFUNCTION(BlueprintCallable)
	void SendInputPressToAbility(const FGameplayTagContainer& GameplayTagContainer);	

	FInputRelease InputRelease;
	FInputPress InputPress;
};
