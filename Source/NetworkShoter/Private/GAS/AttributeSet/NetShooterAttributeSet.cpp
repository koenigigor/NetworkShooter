// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/NetShooterAttributeSet.h"

#include "Net/UnrealNetwork.h"

UNetShooterAttributeSet::UNetShooterAttributeSet()
	:Health(100.f),
	MaxHealth(100.f),
	MinHealth(0.f),
	Armor(100.f),
	MaxArmor(100.f),
	MinArmor(0.f),
	WalkSpeed(500.f),
	MaxWalkSpeed(1000.f),
	MinWalkSpeed(0.f),
	Stamina(1000.f),
	MaxStamina(1000.f),
	MinStamina(0.f)
{
}

void UNetShooterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Health);
	DOREPLIFETIME(ThisClass, Armor);
	DOREPLIFETIME(ThisClass, WalkSpeed);
	DOREPLIFETIME(ThisClass, Stamina);
}

void UNetShooterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNetShooterAttributeSet, Health, OldValue);
}

void UNetShooterAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNetShooterAttributeSet, Armor, OldValue);
}

void UNetShooterAttributeSet::OnRep_WalkSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNetShooterAttributeSet, WalkSpeed, OldValue);
}

void UNetShooterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNetShooterAttributeSet, Stamina, OldValue);
}



//for safe attribute from change, in clamp NevValue sets to 0
//dont know how clamp without notify if current already max/min
void UNetShooterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute,  float& NewValue)
{
	ClampAttribute(Attribute, NewValue);

	Super::PreAttributeChange(Attribute, NewValue);
}

void UNetShooterAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, GetMinHealth(), GetMaxHealth());
	}
	else if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, GetMinArmor(), GetMaxArmor());
	}
	else if (Attribute == GetWalkSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, GetMinWalkSpeed(), GetMaxWalkSpeed());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, GetMinStamina(), GetMaxStamina());
	}	
}


