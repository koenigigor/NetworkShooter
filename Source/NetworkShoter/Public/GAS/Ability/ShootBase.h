// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ShootBase.generated.h"

class AWeapon;

/**
 * Base class for shoot ability
 * Shoot ability must be give player by weapon equipments
 */
UCLASS(Abstract)
class NETWORKSHOTER_API UShootBase : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	void GetShootStartAndDirection(FVector& Start, FVector& Direction, float Length = 1000);
	
	void GetShootStartAndDirectionWithSpread(FVector& Start, FVector& Direction, float Length = 1000);	

	/** Make weapon hit, from muzzle with spread */
	UFUNCTION(BlueprintCallable)
	void MakeHit(FHitResult& OutHit);

	/** Preform one shoot */
	UFUNCTION(BlueprintCallable)
	virtual void MakeShoot() { };
	
	/** return reference on weapon associated with this ability */
	UFUNCTION(BlueprintPure)
	AWeapon* GetAssociatedWeapon();

	/** return trace channel for hit traces */
	virtual ECollisionChannel GetTraceChannel();

	/** Create damage effect spec, set causer and instigator */
	UFUNCTION(BlueprintPure)
	FGameplayEffectSpecHandle MakeDamageEffectSpec();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	FGameplayTag ShootCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
};
