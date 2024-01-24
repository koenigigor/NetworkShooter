// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WaitItemAttributeChange.generated.h"

struct FGameplayTagAttribute; 
class UNSItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWaitItemAttributeChangeHandle, FGameplayTag, Attibute, float, NewValue, float, OldValue);

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UWaitItemAttributeChange : public UAbilityTask
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", GameplayTagFilter="ItemAttribute")) //not filtered?
	static UWaitItemAttributeChange* AbilityTask_WaitItemAttributeChange(UGameplayAbility* OwningAbility, UNSItemInstance* Item, FGameplayTag Attribute, bool TriggerOnce=false);

	UPROPERTY(BlueprintAssignable)
	FWaitItemAttributeChangeHandle OnChange;

	virtual void Activate() override;
	
	void OnAttributeChange(const FGameplayTagAttribute& Attribute);

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UPROPERTY()
	UNSItemInstance* Item = nullptr;
	FGameplayTag AttributeTag;
	bool bTriggerOnce = true;
	
	FDelegateHandle AttributeChangeDelegateHandle;
};
