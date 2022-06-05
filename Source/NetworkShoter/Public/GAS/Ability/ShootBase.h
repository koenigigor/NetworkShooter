// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/NSGameplayAbility_FromEquipment.h"
#include "ShootBase.generated.h"

/**
 * Base class for shoot ability
 * Shoot ability must be give player by weapon equipments
 */
UCLASS(Abstract)
class NETWORKSHOTER_API UShootBase : public UNSGameplayAbility_FromEquipment
{
	GENERATED_BODY()
protected:
	FVector GetMuzzleLocation() const;
	
	void GetShootStartAndDirection(FVector& Start, FVector& Direction, float Length = 1000);
	
	void GetShootStartAndDirectionWithSpread(FVector& Start, FVector& Direction, float Length = 1000);	

	/** Make weapon hit, from muzzle with spread */
	UFUNCTION(BlueprintCallable)
	void MakeHit(FHitResult& OutHit);

	/** Preform one shoot */
	UFUNCTION(BlueprintCallable)
	virtual void MakeShoot() { };

	/** return trace channel for hit traces */
	virtual ECollisionChannel GetTraceChannel();

	/** Create damage effect spec, set causer and instigator */
	UFUNCTION(BlueprintPure)
	FGameplayEffectSpecHandle MakeDamageEffectSpec();

	/** Min spread half angle, lerp on WeaponAttribute.SpreadPercent (0..1) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float SpreadMin = 5.f;

	/** Max spread half angle, lerp on WeaponAttribute.SpreadPercent (0..1) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float SpreadMax = 10.f;
	
	/** larger exponent will cluster points more tightly around the center */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float SpreadExponent = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float ShootDistance = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	FGameplayTag ShootCueTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
};
