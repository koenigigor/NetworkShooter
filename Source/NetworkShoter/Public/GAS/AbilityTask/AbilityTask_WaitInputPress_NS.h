// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitInputPress_NS.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilityTask_WaitInputPress);

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UAbilityTask_WaitInputPress_NS : public UAbilityTask
{
	GENERATED_BODY()
	
	UAbilityTask_WaitInputPress_NS(){};

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_WaitInputPress_NS* AbilityTask_WaitInputPress_NS(UGameplayAbility* OwningAbility, const FGameplayTagContainer& AbilityTags, bool TriggerOnce);
	
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FAbilityTask_WaitInputPress InputPress;

	UFUNCTION()
	void OnInputPress(const FGameplayTagContainer& TagContainer);
	
	bool bTriggerOnce;

	FGameplayTagContainer Tags;
};

