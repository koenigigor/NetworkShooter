// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitProjectileHit.generated.h"

/**
 * Get Actor with projectile component wait when he hit something
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitProjectileHit, const FGameplayAbilityTargetDataHandle, TargetData);

UCLASS()
class NETWORKSHOTER_API UAbilityTask_WaitProjectileHit : public UAbilityTask
{
	GENERATED_BODY()
	
	UAbilityTask_WaitProjectileHit();

	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	/** Get Actor with projectile component and wait when he hit something */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_WaitProjectileHit* AbilityTask_WaitProjectileHit(UGameplayAbility* OwningAbility, AActor* Actor);

	UFUNCTION()
	void ReceiveHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual void TickTask(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FWaitProjectileHit OnHit;
    
    AActor* Projectile = nullptr;
	
	FVector LocationOnPreviousFrame;
};
