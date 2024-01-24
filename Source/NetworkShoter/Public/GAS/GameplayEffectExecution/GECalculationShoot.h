// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GECalculationShoot.generated.h"

/**
 * Damage calculation class
 * decrease armor and health if armor <0
 * ///TODO invent damage formula
 */
UCLASS()
class NETWORKSHOTER_API UGECalculationShoot : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	UGECalculationShoot();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
