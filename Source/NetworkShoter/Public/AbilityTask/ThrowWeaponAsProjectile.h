// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "ThrowWeaponAsProjectile.generated.h"

class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitHit, const FGameplayAbilityTargetDataHandle, TargetData);

/**
 * Get AWeapon actor, launch it as projectile and wait until he hit something
 */
UCLASS()
class NETWORKSHOTER_API UThrowWeaponAsProjectile : public UAbilityTask
{
	GENERATED_BODY()
	UThrowWeaponAsProjectile();

	virtual void Activate() override;

	/** Get AWeapon actor, launch it as projectile and wait until he hit something
	 *  @ThrowingWeapon - weapon actor reference
	 *  @SetRotationAsOwnerView - Set Weapon.rotation = Ability actor rotation
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UThrowWeaponAsProjectile* AbilityTask_ThrowWeaponAsProjectile(UGameplayAbility* OwningAbility, AWeapon* ThrowingWeapon, bool SetRotationAsOwnerView = true);

	virtual void TickTask(float DeltaTime) override;	
	
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(BlueprintAssignable)
	FWaitHit WeaponHit;

	UPROPERTY()
	AWeapon* Weapon = nullptr;

	bool bSetRotationAsOwnerView = true;

	FVector LocationOnPreviousFrame;
};
