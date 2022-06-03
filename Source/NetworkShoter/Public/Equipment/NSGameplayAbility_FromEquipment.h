// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/NSGameplayAbility.h"
#include "NSGameplayAbility_FromEquipment.generated.h"

class UNSEquipmentInstance;

/**
 * Gameplay ability who granted by equipment,
 * get reference on equipment,
 * clear ability on ent if associated equipment no longer equip
 */
UCLASS()
class NETWORKSHOTER_API UNSGameplayAbility_FromEquipment : public UNSGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	UNSEquipmentInstance* GetAssociatedEquipment();

protected:
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	/** Force cancel ability on unequip item */
	UPROPERTY(EditDefaultsOnly)
	bool bForceCancel = false;

	UFUNCTION()
	void OnUnequip(UNSEquipmentInstance* Item);
	bool ItemStillEquipped();
};
