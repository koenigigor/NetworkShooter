// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEEFillAmmo.generated.h"

/**
 * Set current ammo to max ammo
 * set from bp gameplay effect not work
 * (capture max ammo attribute of first call and not change if swap weapon)
 */
UCLASS()
class NETWORKSHOTER_API UGEEFillAmmo : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	UGEEFillAmmo();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
