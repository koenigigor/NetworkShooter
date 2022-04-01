// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_DoExplosiveDamage.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCalculateExplosiveDamage, FGameplayEffectSpecHandle, SpecHandle, FGameplayAbilityTargetDataHandle, TargetData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCalculateExplosiveDamageFinishExecute, FGameplayEffectSpecHandle, SpecHandle, FGameplayAbilityTargetDataHandle, TargetData);

/**
 * Calculate explosive damage based on radius
 */
UCLASS()
class NETWORKSHOTER_API UAbilityTask_DoExplosiveDamage : public UAbilityTask
{
	GENERATED_BODY()

	/** Calculate explosive damage based on radius
	 *	@ExlodeLocation Center of explode 
	 *	@ExplodeDamping Curve from 1 to 0, explode force by distance
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_DoExplosiveDamage* AbilityTask_DoExplosiveDamage(UGameplayAbility* OwningAbility, FVector ExplodeLocation,
		float ExplodeRadius, float ExplodeBaseDamage, TSubclassOf<UGameplayEffect> DamageGameplayEffectClass, UCurveFloat* ExplodeDamping = nullptr);
	
	FVector Location;
	float Radius = 0.f;
	float BaseDamage = 0.f;
	UCurveFloat* Damping = nullptr;

	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(BlueprintAssignable)
	FCalculateExplosiveDamage Damaged;
	
	UPROPERTY(BlueprintAssignable)
	FCalculateExplosiveDamageFinishExecute FinishExecute;
	
	virtual void Activate() override;
};
