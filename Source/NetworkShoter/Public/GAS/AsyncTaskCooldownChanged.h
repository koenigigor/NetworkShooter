// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncTaskCooldownChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCooldownChanged, FGameplayTag, CooldownTag, float, TimeRemaining, float, Duration);

/**
 * Blueprint node to automatically register a listener for changes (Begin and End) to an array of Cooldown tags.
 * Useful to use in UI.
 *
 * Can Bind to many different cooldowns
 *
 * Copy from GAG documentation
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy))
class NETWORKSHOTER_API UAsyncTaskCooldownChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownBegin;

	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownEnd;

	/** Listen for change cooldown (Begin/End)
	 *  If using ServerCooldown, TimeRemaining and Duration will return -1 to signal local predicted cooldown has begun. */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true"))
	static UAsyncTaskCooldownChanged* ListenForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer CooldownTag, const bool UseServerCooldown);

	/** !!!! Must be class manually (in widget destruct), or we get memory leak	*/
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystem = nullptr;

	FGameplayTagContainer CooldownTags;

	bool bUseServerCooldown;

	/** Cooldown Begin if Effect added */
	void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	/** Cooldown Ends if tag removed */
	void CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount);

	/** Get Remaining Time and total duration, if has multiple cooldowns with same tag, return longest */
	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTag, float& TimeRemaining, float& CooldownDuration);
};
