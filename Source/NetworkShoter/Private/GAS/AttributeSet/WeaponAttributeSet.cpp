// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/WeaponAttributeSet.h"

#include "Net/UnrealNetwork.h"

UWeaponAttributeSet::UWeaponAttributeSet():
	WeaponDamage(0.f),
	//Ammo(0.f),
	MaxAmmo(0.f),
	ShootsPerSec(1.f),
	ReloadTime(0.f),
	SpreadPercent(0.f),
	MeleePerSecond(1.f)
{
}

void UWeaponAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UWeaponAttributeSet, WeaponDamage, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(UWeaponAttributeSet, Ammo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponAttributeSet, MaxAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponAttributeSet, ShootsPerSec, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponAttributeSet, ReloadTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponAttributeSet, MeleePerSecond, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponAttributeSet, SpreadPercent, COND_OwnerOnly);
}

void UWeaponAttributeSet::CopyFrom(const UWeaponAttributeSet* Other)
{
	WeaponDamage = Other->WeaponDamage;
	//Ammo = Other->Ammo;
	MaxAmmo = Other->MaxAmmo;
	ShootsPerSec = Other->ShootsPerSec;
	ReloadTime = Other->ReloadTime;
	MeleePerSecond = Other->MeleePerSecond;
}

void UWeaponAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	ClampAttribute(Attribute, NewValue);
	
	Super::PreAttributeChange(Attribute, NewValue);
}

void UWeaponAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetSpreadPercentAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
	}
}
