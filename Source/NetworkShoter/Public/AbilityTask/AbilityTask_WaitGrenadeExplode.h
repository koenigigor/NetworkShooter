// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitGrenadeExplode.generated.h"

/**
 * Ability Task for check projectile location 
 * and notify when they must blow up
 */

class AWeapon;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitExplodeDelegate, const FGameplayAbilityTargetDataHandle&, TargetData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitExplodeDelegate, const FVector&, Location);

UCLASS()
class NETWORKSHOTER_API UAbilityTask_WaitGrenadeExplode : public UAbilityTask
{
	GENERATED_BODY()
//public:
	UAbilityTask_WaitGrenadeExplode();
	
	UPROPERTY(BlueprintAssignable)
	FWaitExplodeDelegate OnExplode;

	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_WaitGrenadeExplode* WaitForExplode(UGameplayAbility* OwningAbility, AWeapon* GrenadeActor);
	
	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION()
	void OnExplodeCallback(FVector Location);

	UPROPERTY()
	AWeapon* Grenade;

	FVector LocationOnPreviousFrame;

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual void TickTask(float DeltaTime) override;
};
