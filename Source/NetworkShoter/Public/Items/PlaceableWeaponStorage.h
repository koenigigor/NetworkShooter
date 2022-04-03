// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlaceableWeaponStorage.generated.h"

class APlaceableWeapon;

/**
 * Actor component who store placeable weapons and spawn it in world
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UPlaceableWeaponStorage : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlaceableWeaponStorage();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	bool IsInPlacingMode();
	
protected:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StartPlaceWeapon();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void FinishPlaceWeapon();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void CancelPlaceWeapon();
	
	void UpdatePlaceLocation();

	/** if cant place here, change material */
	void CheckCanBePlaced();

	/** Can place weapon on this location or no,
	 *  tick asking in place mode	*/
	bool CanPlace();

	UFUNCTION()
	void OnRep_WeaponToPlace();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlaceableWeapon> StoredWeaponClass;

	UPROPERTY(ReplicatedUsing="OnRep_WeaponToPlace")
	APlaceableWeapon* WeaponToPlace;

	UPROPERTY()
	APawn* OwningPawn;

	bool bCanPlaceOnPreviousFrame = true;
};
