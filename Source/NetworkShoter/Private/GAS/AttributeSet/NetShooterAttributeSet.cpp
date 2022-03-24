// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/NetShooterAttributeSet.h"

#include "Net/UnrealNetwork.h"

UNetShooterAttributeSet::UNetShooterAttributeSet()
	:Health(100.f),
	MaxHealth(100.f),
	MinHealth(0.f),
	Armor(100.f),
	MaxArmor(100.f),
	MinArmor(0.f)
{
}

void UNetShooterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNetShooterAttributeSet, Health);
	DOREPLIFETIME(UNetShooterAttributeSet, Armor);
}

void UNetShooterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNetShooterAttributeSet, Health, OldValue);
}

void UNetShooterAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNetShooterAttributeSet, Armor, OldValue);
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


