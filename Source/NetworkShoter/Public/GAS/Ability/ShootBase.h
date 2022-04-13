// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ShootBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class NETWORKSHOTER_API UShootBase : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable)
	void GetShootStartAndDirection(FVector& Start, FVector& Direction, float Length = 1000);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ShootCueTag;
};
