// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitInputRelease_NS.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitInputReleaseDelegateSignature);

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UAbilityTask_WaitInputRelease_NS : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAbilityTask_WaitInputRelease_NS(){};

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_WaitInputRelease_NS* AbilityTask_WaitInputRelease_NS(UGameplayAbility* OwningAbility, const FGameplayTagContainer& AbilityTags, bool TriggerOnce);
	
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
	FWaitInputReleaseDelegateSignature InputRelease;

	UFUNCTION()
	void OnInputRelease(const FGameplayTagContainer& TagContainer);
	
	bool bTriggerOnce;

	FGameplayTagContainer Tags;
};
