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
	UFUNCTION(BlueprintCallable)
	void GetShootStartAndDirection(FVector& Start, FVector& Direction, float Length = 1000);

	UFUNCTION(BlueprintCallable)
	void GetShootStartAndDirectionWithSpread(FVector& Start, FVector& Direction, float Length = 1000);	
	
	/** return reference on weapon associated with this ability */
	UFUNCTION(BlueprintPure)
	AWeapon* GetAssociatedWeapon();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	FGameplayTag ShootCueTag;
};
