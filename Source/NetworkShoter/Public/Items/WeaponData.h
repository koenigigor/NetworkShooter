// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AttributeSet/WeaponAttributeSet.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	UTexture2D* UIImage;

	/** Original weapon skill
	*	slice for sword,
	*	shoot for rifle, etc.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability", meta=(EditCondition="Type!=EWeaponType::Grenade", EditConditionHides))
	TSubclassOf<UGameplayAbility> PrimaryAbility;

	/** Second Weapon skill
	 * Block or punch for sword
	 * punch for rifle, etc
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability", meta=(EditCondition="Type!=EWeaponType::Grenade", EditConditionHides))
	TSubclassOf<UGameplayAbility> SecondaryAbility;

	/** Throw weapon as projectile
	 *  Last word in hopeless situation =)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability", meta=(EditCondition="Type!=EWeaponType::Grenade", EditConditionHides))
	TSubclassOf<UGameplayAbility> Throw;

	/** Initial weapon attribute set for  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability", meta=(EditCondition="Type!=EWeaponType::Grenade", EditConditionHides))
	UDataTable* AttributeSet;

///Range settings
	/** Shoot by projectile ability get projectile info here */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Range")
	bool bShootByProjectile = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Range", meta=(EditCondition="bShootByProjectile"))
	float ProjectileSpeed = 1300;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Range", meta=(EditCondition="bShootByProjectile"))
	TSubclassOf<AActor> ProjectileClass = nullptr;

///Throw settings (grenades like)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Throw");
	bool bExplodeOnDestroy = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Throw", meta=(EditCondition="bExplodeOnDestroy"));
	float ExplodeRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Throw");
	float ExplodeDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Throw");
	float ThrowSpeed = 1000.f;
};
