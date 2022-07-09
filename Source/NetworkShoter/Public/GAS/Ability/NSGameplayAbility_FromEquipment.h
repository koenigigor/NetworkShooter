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
	UNSGameplayAbility_FromEquipment();
	
	UFUNCTION(BlueprintPure)
	UNSEquipmentInstance* GetAssociatedEquipment() const;
	AActor* GetAssociatedEquipmentActor() const;
	AActor* GetAssociatedEquipmentActor_Ensured() const;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	bool IsForceCancel() const { return bForceCancel; };
protected:
	/** Force cancel ability on unequip item */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	bool bForceCancel = false;

	UPROPERTY(EditDefaultsOnly, Category = "Costs", meta=(Categories="ItemAttribute"))
	TMap<FGameplayTag, float> ItemAttributeCost;
	
	bool ItemStillEquipped();
};
