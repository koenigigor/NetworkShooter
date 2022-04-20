// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/ShootBase.h"
#include "ShootProjectile.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UShootProjectile : public UShootBase
{
	GENERATED_BODY()

	virtual void MakeShoot() override;
	
	UFUNCTION(BlueprintCallable)
	FVector SuggestProjectileVelocity();

	virtual ECollisionChannel GetTraceChannel() override;

	void SpawnProjectile(FVector Velocity);
};
