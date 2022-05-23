// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NSGameplayAbility_FromEquipment.generated.h"

class UNSEquipmentInstance;

/**
 * Gameplay ability who granted by equipment,
 * get reference on equipment,
 * clear ability on ent if associated equipment no longer equip
 * //todo switch force end on unequip?
 */
UCLASS()
class NETWORKSHOTER_API UNSGameplayAbility_FromEquipment : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UNSEquipmentInstance* GetAssociatedEquipment();

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	bool ItemStillEquipped();
};
