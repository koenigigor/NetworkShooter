// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttributeSet.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	None,
	RangeWeapon,
	MeleeWeapon,
	Grenade
};

/**
 * DataAsset for all weapon data
 */
UCLASS(BlueprintType)
class NETWORKSHOTER_API UWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
///General Info settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	EWeaponType Type;
	
	/** Weapon mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	UStaticMesh* Mesh = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	FName Name = "Weapon name placeholder";

	/** Original weapon skill
	*	slice for sword,
	*	shoot for rifle, etc.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability")
	TSubclassOf<UGameplayAbility> PrimaryAbility;

	/** Second Weapon skill
	 * Block or punch for sword
	 * punch for rifle, etc
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability")
	TSubclassOf<UGameplayAbility> SecondaryAbility;

	/** Throw weapon as projectile
	 *  Last word in hopeless situation =)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability")
	TSubclassOf<UGameplayAbility> Throw;

	/** Initial weapon attribute set for  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability")
	UDataTable* AttributeSet;

///Range settings	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Range")
	FName MuzzleSocket = "Muzzle";

///Explode settings (grenades like)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Explode");
	bool bExplodeOnDestroy = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Explode");
	float ExplodeRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Explode");
	float ExplodeDamage;
};
