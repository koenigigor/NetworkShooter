// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ObserveAttributeThreshold.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FObserveAttributeDelegate, float, OldValue, float, NewValue);

/**
 * Observe attribute change in range min max values,
 * for regen type abilities
 */
UCLASS()
class NETWORKSHOTER_API UObserveAttributeThreshold : public UAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FObserveAttributeDelegate AttributeChangeInRange;
	UPROPERTY(BlueprintAssignable)
	FObserveAttributeDelegate UpThresholdReach;
	UPROPERTY(BlueprintAssignable)
	FObserveAttributeDelegate DownThresholdReach;

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	/** Wait on attribute change in selected range */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UObserveAttributeThreshold* ObserveAttributeThreshold(UGameplayAbility* OwningAbility, FGameplayAttribute ObservedAttribute, float UpThresholdValue, float DownThresholdValue, bool TriggerOnce, AActor* OptionalExternalOwner = nullptr);

protected:
	void OnAttributeChange(const FOnAttributeChangeData& CallbackData);

	UAbilitySystemComponent* GetFocusedASC();

	
	float bTriggerOnce = false;
	
	FGameplayAttribute Attribute;
	float UpThreshold;
	float DownThreshold;

	UPROPERTY()
	AActor* ExternalOwner = nullptr;
	
	FDelegateHandle OnAttributeChangeDelegateHandle;
};
