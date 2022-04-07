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

	virtual void Tick(float DeltaSeconds) override;
	
	/** UPlaceableWeaponStorage call this when weapon finally placed in world */
	UFUNCTION(BlueprintCallable)
	bool FinishPlaceWeapon();

	UFUNCTION(NetMulticast, Unreliable)
	void FinishPlaceNotify();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_FinishPlaceWeapon();

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TSubclassOf<AActor> SpawnedActor = nullptr;

	/** Can place weapon on this location or no	*/
	bool CanPlace();
	
protected:	
	//toggle Material in placing mode
	void ToggleMaterial(bool bNormal);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ToggleMaterial(bool bNormal);
	
	/** if cant place here, change material */
	void CheckCanBePlaced();

	void UpdatePlaceLocation();

protected:
	bool bCanPlaceOnPreviousFrame = true;
};
