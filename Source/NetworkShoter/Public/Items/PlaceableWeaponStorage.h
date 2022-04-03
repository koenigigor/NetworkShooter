// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlaceableWeaponStorage.generated.h"

class APlaceableWeapon;
class USpringArmComponent;

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

protected:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void StartPlaceWeapon();

	UFUNCTION(BlueprintCallable)
	void FinishPlaceWeapon();

	UFUNCTION(BlueprintCallable)
	void CancelPlaceWeapon();
	
	void UpdatePlaceLocation();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlaceableWeapon> StoredWeaponClass;

	UPROPERTY(Replicated)
	APlaceableWeapon* PlacedWeapon;
	
	UPROPERTY(Replicated)
	USpringArmComponent* Arm;
};
