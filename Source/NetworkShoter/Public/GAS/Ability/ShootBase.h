// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/NSGameplayAbility_FromEquipment.h"
#include "ShootBase.generated.h"

/**
 * Base class for shoot ability
 * Shoot ability must be give player by weapon equipments
 */
UCLASS(Abstract)
class NETWORKSHOTER_API UShootBase : public UNSGameplayAbility_FromEquipment
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	FVector GetMuzzleLocation() const;
	
	void GetShootStartAndDirection(FVector& Start, FVector& Direction, float Length = 1000);
	
	void GetShootStartAndDirectionWithSpread(FVector& Start, FVector& Direction, float Length = 1000);	

	/** Make weapon hit, from muzzle with spread */
	UFUNCTION(BlueprintCallable)
	void MakeHit(FHitResult& OutHit);

	UFUNCTION(BlueprintCallable)
	void MakeShoot();
	
	/** Preform single shoot */
	UFUNCTION(BlueprintCallable)
	virtual void MakeSingleShoot();

	/** return trace channel for hit traces */
	virtual ECollisionChannel GetTraceChannel();

	/** delay after shoot (shooting speed) */
	UFUNCTION(BlueprintPure)
	float GetShootDelay() const;
	
	/** Create damage effect spec, set causer and instigator */
	UFUNCTION(BlueprintPure)
	FGameplayEffectSpecHandle MakeDamageEffectSpec() const;

	//todo maybe remove see "bAddShootCue"
	enum class EQueStage
	{
		ActivateAbility,
		MakeShoot,
		EndAbility
	};
	//todo maybe remove see "bAddShootCue"
	void ProcessShootQue(EQueStage Stage) const;

	/** How mush bullets fly per shoot (for shotgun, etc.) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	int32 BulletsPerShoot = 1;

	/** Spread half angle, lerp on WeaponAttribute.SpreadPercent (0..1) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup | Spread")
	FVector2D Spread = {5.0, 10.0};
	
	/** larger exponent will cluster points more tightly around the center */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup | Spread")
	float SpreadExponent = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float ShootDistance = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
    TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;	

	/**	if true Add cue on ability activate and remove on end
	 *	if false, Execute cue once on shoot
	 *
	 *	UNSUPPORTED before want add/remove cue on start and end fire, with ability "wait input release"
	 *		but in GASShooter and Lyra Cue execute every shoot
	 */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup | Cue")
	bool bAddShootCue = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup | Cue", meta=(Categories="GameplayCue"))
	FGameplayTag ShootCueTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup | Cue", meta=(Categories="GameplayCue"))
	FGameplayTag ImpactCue;
};
