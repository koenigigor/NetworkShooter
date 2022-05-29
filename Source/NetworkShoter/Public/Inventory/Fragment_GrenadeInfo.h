// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/NSItemDefinition.h"
#include "Fragment_GrenadeInfo.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UFragment_GrenadeInfo : public UNSInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade")
	TSubclassOf<AActor> ProjectileClass = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade");
	bool bExplode = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade", meta=(EditCondition="bExplode"));
	float ExplodeRadius = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade", meta=(EditCondition="bExplode"))
	UCurveFloat* ExplodeDamageFalloff = nullptr;	//todo thing about FRuntimeFloatCurve

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade");
	float ExplodeDamage = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade");
	float ThrowSpeed = 1000.f;
};
