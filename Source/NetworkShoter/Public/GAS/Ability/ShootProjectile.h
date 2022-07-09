// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Ability/ShootBase.h"
#include "ShootProjectile.generated.h"

class ANSProjectile;

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UShootProjectile : public UShootBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	float ProjectileSpeed = 1300.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	TSubclassOf<ANSProjectile> ProjectileClass = nullptr;
	
	virtual void MakeSingleShoot() override;
	
	UFUNCTION(BlueprintCallable)
	FVector SuggestProjectileVelocity();

	virtual ECollisionChannel GetTraceChannel() override;

	void SpawnProjectile(FVector Velocity);
};
