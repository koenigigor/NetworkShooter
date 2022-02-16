// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NSEquipment.generated.h"

class AWeapon;
class AGrenade;
class UWeaponData;

/** store class collected grenades and their count */
USTRUCT(BlueprintType)
struct FGrenadeCount
{
	GENERATED_BODY()
	FGrenadeCount(){};
	
	FGrenadeCount(UWeaponData* Data, int32 ItemCount)
	{
		GrenadeData = Data;
		Count = ItemCount;
	};

	//store weapon data for construct grenades when it nesesary
	UPROPERTY()
	UWeaponData* GrenadeData = nullptr;
	
	int32 Count = 1;
};

/** Network Shooter base equipment component
 *	Collect and store grenades
 *	Switch weapon
 *	Provides 4 grenade slot and 4 weapon slot
 *	
 */

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UNSEquipment : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNSEquipment();
	
	UFUNCTION(BlueprintCallable)
	bool PickUpWeapon(AWeapon* Weapon);
	
	/** try Equip weapon */
	bool EquipWeapon(int32 Slot);

	/** Remove 1 grenade */
//	TSubclassOf<AGrenade> RemoveGrenade(int32 Slot);

	/** Return Grenade ptr in selected slot*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeapon* GetGrenade(int32 Slot, bool bWithRemove=true);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeapon* GetSelectedGrenade(bool bWithRemove=true);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** Create and attach default weapon */
	void CreateDefaultWeapon();

public:
	/** Default weapon created when game start */
	UPROPERTY(EditDefaultsOnly)
	UWeaponData* DefaultWeapon;

protected:
	/** Store weapons */
	UPROPERTY()
	TArray<AWeapon*> Weapons;

	/** Store Grenades */
	UPROPERTY()
	TArray<FGrenadeCount> Grenades;

	UPROPERTY()
	int32 SelectedGrenadeSlot = 0;
};
