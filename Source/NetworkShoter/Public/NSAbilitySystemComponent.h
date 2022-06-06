// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NSAbilitySystemComponent.generated.h"

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
	 *
	 * binds to enhanced input in Lyra HeroComponent
	 * LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased
	 */
	void InputTagPressed(const FGameplayTag& Tag);
	void InputTagReleased(const FGameplayTag& Tag);

	/* tick, called from NSPlayerController::PostProcessInput
	 * tries activate ability each frame while button is pressed */
	void ProcessInputHoldAbilities();

private:
	/** stored abilities who input still pressed */
	TMap<FGameplayTag, FGameplayAbilitySpecHandle> AbilitiesWhoWaitInputRelease;

	/* is ability activated while input hold? */
	bool IsInputHoldAbility(const FGameplayAbilitySpecHandle& Handle);
};
