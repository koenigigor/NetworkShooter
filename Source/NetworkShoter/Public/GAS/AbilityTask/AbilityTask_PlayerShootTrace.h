// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PlayerShootTrace.generated.h"

/**
 * Do line trace from weapon muzzle, and return target data
 */

class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitShootResult, const FGameplayAbilityTargetDataHandle, TargetData);

UCLASS()
class NETWORKSHOTER_API UAbilityTask_PlayerShootTrace : public UAbilityTask
{
	GENERATED_BODY()
	
	//UAbilityTask_PlayerShootTrace();

	UPROPERTY(BlueprintAssignable)
	FWaitShootResult WaitShootResult;

	virtual void Activate() override;

	/** Do line trace from weapon muzzle, and return target data */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_PlayerShootTrace* AbilityTask_PlayerShootTrace(UGameplayAbility* OwningAbility, float Distantion);

	float ShootDistantion = -1;
};
