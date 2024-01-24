// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/NSEquipmentDefinition.h"
#include "EFragment_Ability.generated.h"

class UNSGameplayAbility_FromEquipment;
class UAbilitySystemComponent;

/**
 * Fragment responses for give ability to owner
 */
UCLASS()
class NETWORKSHOTER_API UEFragment_Ability : public UNSEquipmentFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UNSGameplayAbility_FromEquipment>> AbilitiesToGrant;

	virtual void OnEquip(UNSEquipmentInstance* Instance) override;
	virtual void OnUnequip(UNSEquipmentInstance* Instance) override;

private:
	UAbilitySystemComponent* GetOwnerASC(AActor* Owner) const;
};
