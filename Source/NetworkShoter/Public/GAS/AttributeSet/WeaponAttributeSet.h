// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "WeaponAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Attribute set for collect equipped weapon attributes
 * if owner character must be set when weapon equip
 */
UCLASS()
class NETWORKSHOTER_API UWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UWeaponAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Copy params from other WeaponAttributeSet */
	void CopyFrom(const UWeaponAttributeSet* Other);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue);
	
	/** Weapon parameters, must be set when weapon equip */ //TODO check if can make 2 attribute set in character? make weapon attribute set

	/** RangeWeapon parameters */
	UPROPERTY(BlueprintReadOnly, Replicated, Category="Weapon|Attribute|Range")
	FGameplayAttributeData WeaponDamage;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, WeaponDamage)

//	UPROPERTY(BlueprintReadOnly, Replicated, Category="Weapon|Attribute|Range")
//	FGameplayAttributeData Ammo;
//	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, Ammo)

	UPROPERTY(BlueprintReadOnly, Replicated, Category="Weapon|Attribute|Range")
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, MaxAmmo)

	UPROPERTY(BlueprintReadOnly, Replicated, Category="Weapon|Attribute|Range")
	FGameplayAttributeData ShootsPerSec;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, ShootsPerSec)

	UPROPERTY(BlueprintReadOnly, Replicated, Category="Weapon|Attribute|Range")
	FGameplayAttributeData ReloadTime;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, ReloadTime)

	/** 0..1 spread coefficient in abilities (in shoot response for lerp between min and max values) */
	UPROPERTY(BlueprintReadOnly, Replicated, Category="Weapon|Attribute|Range")
	FGameplayAttributeData SpreadPercent;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, SpreadPercent)

	/** Melee Parameters */
	UPROPERTY(BlueprintReadOnly, Replicated, Category="Weapon|Attribute|Melee")
	FGameplayAttributeData MeleePerSecond;
	ATTRIBUTE_ACCESSORS(UWeaponAttributeSet, MeleePerSecond)

	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Melee")
	FGameplayAttributeData MeleeDamage;
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Melee")
	FGameplayAttributeData PunchDestination;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Melee")
	FGameplayAttributeData PunchRadius;
	*/
};
