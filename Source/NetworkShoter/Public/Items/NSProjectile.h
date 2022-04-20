// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAS/MyGameplayEffectSpec.h"
#include "NSProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class NETWORKSHOTER_API ANSProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ANSProjectile();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn))
	FGameplayEffectSpecHandle DamageEffectHandle;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCollision = nullptr;
};
