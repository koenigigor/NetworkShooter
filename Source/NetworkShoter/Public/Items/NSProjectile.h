// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GAS/AbilityBPLibrary.h"
#include "NSProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class NETWORKSHOTER_API ANSProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ANSProjectile();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn))
	FGameplayEffectSpecHandle DamageEffectHandle;
	
	UPROPERTY(BlueprintReadOnly, Replicated, meta=(ExposeOnSpawn), meta=(Categories="GameplayCue"))
	FGameplayTag ImpactCue;

	UPROPERTY(BlueprintReadOnly, Replicated, meta=(ExposeOnSpawn))
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn))
	UAbilitySystemComponent* SourceASC = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovement = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCollision = nullptr;
};
