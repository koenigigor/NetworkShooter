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

	/** Can place weapon on this location or no,
	 *  tick asking in place mode	*/
	bool CanPlace();

	/** Called on start placing, change material, collision */
	void StartPlacingWeapon();
	
	/** Try finally place weapon */
	bool PlaceWeapon();

	/** Hard end place weapon, if cant spawn escape from placing made */
	void CancelPlacing();
	
	void OnPlaced();
};
