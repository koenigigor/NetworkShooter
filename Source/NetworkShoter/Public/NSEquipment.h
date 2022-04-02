// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NSEquipment.generated.h"

class AWeapon;
class AGrenade;
class UWeaponData;

/** [client] call rep notify */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponUpdated);
/** [client] and [server] */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

/** Struct for stored grenades and their count */
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

	//store weapon data for construct grenades when it need
	UPROPERTY()
	UWeaponData* GrenadeData = nullptr;
	
	int32 Count = 1;
};

/** Network Shooter base equipment component
 *	Collect and store grenades and weapons
 *	Switch, equip, unequip weapon
 *	defaults: 4 grenade slot and 4 weapon slot
 *		equip DefaultWeapon on BeginPlay
 */

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UNSEquipment : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNSEquipment();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/** [Server] spawn and equip default weapon */
	virtual void BeginPlay() override;

	
	//~==============================================================================================
	// Pickup 
public:
	/** [Server] Pick weapon from ground, add it in storage*/
	UFUNCTION(BlueprintCallable)
	bool PickUpWeapon(AWeapon* Weapon);
	
	/** [Server] Drop weapon to world
	 *	@return Dropped weapon */
	UFUNCTION(BlueprintCallable)
	AWeapon* DropCurrentWeapon();
	
	/** [Server] try Equip weapon */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void EquipWeapon(int32 Slot);

	/** [Client or Server] try equip weapon in next slot */
	UFUNCTION(BlueprintCallable)
	void EquipNextWeapon(bool Up=true);

protected:
	/** [Server] Unregister abilities and detach weapon from character */
	AWeapon* UnequipWeapon(bool bAddInStorage = true);

	/** apply new weapon stats and register weapon ability */
	void RegisterWeaponAbilities(AWeapon* Weapon);
	void UnregisterWeaponAbilities(AWeapon* Weapon);


	//~==============================================================================================
	// Getters
public:	
	/** Get Equipped weapon ref */
    UFUNCTION(BlueprintCallable, BlueprintPure)
    AWeapon* GetEquippedWeapon() {return EquippedWeapon; };
	
	/** Get Equipped weapon slot */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetEquippedWeaponSlot() { return EquippedWeaponSlot; };

	/** Get all weapons ref */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AWeapon*> GetAllWeapons() { return Weapons; };
	
	/** [Server] Return Grenade ptr in selected slot*/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeapon* GetGrenade(int32 Slot, bool bWithRemove=true);

	/** [Server] ReturnGrenade in selected slot */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeapon* GetSelectedGrenade(bool bWithRemove=true);

	
private:
	UFUNCTION()
	void OnRep_Weapons();
	UPROPERTY(BlueprintAssignable)
	FWeaponUpdated WeaponUpdated;

	UFUNCTION()
	void OnRep_SelectWeapon();
	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;
	
protected:
	/** Default weapon created when game start */
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	UWeaponData* DefaultWeapon;

	/** Count weapons who can be stored */
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	int32 MaxWeaponSlots = 3;

	
	/** Store weapons */
	UPROPERTY(ReplicatedUsing=OnRep_Weapons)
	TArray<AWeapon*> Weapons;

	/** Store Grenades */
	UPROPERTY()
	TArray<FGrenadeCount> Grenades;

	
	UPROPERTY()
	int32 SelectedGrenadeSlot = 0;

	UPROPERTY(Replicated)
	AWeapon* EquippedWeapon = nullptr;
	
	/** Slot where weapon current equip */
	UPROPERTY(ReplicatedUsing=OnRep_SelectWeapon)
	int32 EquippedWeaponSlot = -1;
};
