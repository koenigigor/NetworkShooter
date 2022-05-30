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
	if (Attribute == GetHealthAttribute())
	{
		ClampAttribute(GetHealthAttribute(), NewValue, MinHealth, MaxHealth);
	}

	if (Attribute == GetArmorAttribute())
	{
		ClampAttribute(GetArmorAttribute(), NewValue, MinArmor, MaxArmor);
	}

	if (Attribute == GetWalkSpeedAttribute())
	{
		ClampAttribute(GetWalkSpeedAttribute(), NewValue, MinWalkSpeed, MaxWalkSpeed);
	}

	if (Attribute == GetStaminaAttribute())
	{
		ClampAttribute(GetStaminaAttribute(), NewValue, MinStamina, MaxStamina);
	}	

	Super::PreAttributeChange(Attribute, NewValue);
}

void UNetShooterAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute,
		float& NewValue ,const FGameplayAttributeData& Min, const FGameplayAttributeData& Max)
{
	float MaxValue = Max.GetCurrentValue();
	float MinValue = Min.GetCurrentValue();
	
	if (NewValue > MaxValue)
	{
		GetOwningAbilitySystemComponent()->ApplyModToAttribute(Attribute, EGameplayModOp::Override, MaxValue); //without damage calculation not border by 0

		NewValue = MaxValue;
	}

	if (NewValue < MinValue)
	{
		GetOwningAbilitySystemComponent()->ApplyModToAttribute(Attribute, EGameplayModOp::Override, MinValue);

		NewValue = MinValue;
	}
}


