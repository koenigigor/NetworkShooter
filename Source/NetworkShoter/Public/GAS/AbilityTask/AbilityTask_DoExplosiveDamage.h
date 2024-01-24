// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_DoExplosiveDamage.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCalculateExplosiveDamage);

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
		float ExplodeRadius, float ExplodeBaseDamage, TSubclassOf<UGameplayEffect> DamageGameplayEffectClass, AActor* DamageCauser = nullptr, UCurveFloat* ExplodeDamping = nullptr);
	
	FVector Location;
	float Radius = 0.f;
	float BaseDamage = 0.f;
	
	UPROPERTY()
	UCurveFloat* Damping = nullptr;

	UPROPERTY()
	AActor* Causer = nullptr;

	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(BlueprintAssignable)
	FCalculateExplosiveDamage FinishExecute;
	
	virtual void Activate() override;
};
