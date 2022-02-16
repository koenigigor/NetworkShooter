// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GE_ExplodeCalculation.generated.h"

/**
 * Explosive damage calculation
 * ignore armor
 */
UCLASS()
class NETWORKSHOTER_API UGE_ExplodeCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	UGE_ExplodeCalculation();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
