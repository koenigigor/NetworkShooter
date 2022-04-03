// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlaceableWeapon.generated.h"


/**
 * BaseClass for weapons who can be placed in world
 * (Turret, Mine, etc.)
 */

UCLASS()
class NETWORKSHOTER_API APlaceableWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	APlaceableWeapon();
	
	/** UPlaceableWeaponStorage call this when weapon finally placed in world */
	UFUNCTION(NetMulticast, Reliable)
	void FinishPlaceWeapon();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_FinishPlaceWeapon();

	//toggle Material in placing mode
	void ToggleMaterial(bool bNormal);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ToggleMaterial(bool bNormal);
};
