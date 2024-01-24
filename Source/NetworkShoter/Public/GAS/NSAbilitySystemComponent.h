// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NSAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FServerReplicateSubclassDelegate, FGameplayAbilitySpecHandle AbilityHandle, const TSubclassOf<UObject> ObjectClass)

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UNSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	virtual int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) override;

public:
	/** Add/Remove ability for wants to activate,
	 *	bActivateAbility - Activate Ability, or send press/release events only
	 *
	 * binds to enhanced input in Lyra HeroComponent
	 * LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased
	 */
	UFUNCTION(BlueprintCallable)
	void InputTagPressed(const FGameplayTag& Tag, bool bActivateAbility = true);
	UFUNCTION(BlueprintCallable)
	void InputTagReleased(const FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm ="Tag"))
	bool AbilityIsActive(const FGameplayTagContainer& Tag);
	
public:
	/** Send gameplay event without payload on server */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerReplicateGameplayEvent(const FGameplayTag& Tag);

	/** used by UAbilityTask_SendSubclassOnServer */
	UFUNCTION(Server, Reliable)
	void ServerReplicateSubclass(FGameplayAbilitySpecHandle AbilityHandle, TSubclassOf<UObject> ObjectClass);
	FServerReplicateSubclassDelegate OnClassReplicate;
	
	/* tick, called from NSPlayerController::PostProcessInput
	 * tries activate ability each frame while button is pressed */
	void ProcessInputHoldAbilities();

private:
	/** stored abilities who input still pressed */
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> AbilitiesWhoWaitInputRelease;

	/* is ability activated while input hold? */
	bool IsInputHoldAbility(const FGameplayAbilitySpecHandle& Handle);
};
