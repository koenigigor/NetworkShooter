// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NSGameplayAbility.generated.h"

class UNSItemDefinition;

UENUM()
enum class EActivationPolicy : uint8
{
	InputPress,
	InputHold,
	ActivateOnGrant
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

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	void TryActivateOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	/** Additional Inventory item cost */
	UPROPERTY(EditDefaultsOnly, Category = "Costs")
	TMap<TSubclassOf<UNSItemDefinition>, int32> ItemCost;
	
	UPROPERTY(EditDefaultsOnly)
	EActivationPolicy ActivationPolicy;
};
