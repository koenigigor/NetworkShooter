// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/NetShooterAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "NSStructures.h"
#include "Game/NSGameMode.h"
#include "Game/NSGameState.h"
#include "Game/Components/ChatController.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
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

//void ULyraHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
void UNetShooterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
//[Server]	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const auto Causer = Data.EffectSpec.GetEffectContext().GetEffectCauser();
		const auto Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();
		const auto Target = GetOwningActor();
		const auto Damage = Data.EvaluatedData.Magnitude;
		const auto Ability = Data.EffectSpec.GetEffectContext().GetAbility();
			
		ensure(Causer);
		ensure(Instigator);

		const auto InstigatorState = Cast<APlayerState>(Instigator);
		if (!InstigatorState)
		{
			UE_LOG(LogTemp, Warning, TEXT("Intend instigator is PlayerState, in %s"), Ability ? *Ability->GetName() : *Causer->GetName())
		}
        			
		FDamageInfo DamageInfo(InstigatorState, Causer, Target, Damage, Ability);

		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
		
		
		if (Data.EvaluatedData.Magnitude < 0.f)
		{		
			DamageInfo.Tag = NSTag::System::Damage();
			MessageSystem.BroadcastMessage(DamageInfo.Tag, DamageInfo);
			
			if (FMath::IsNearlyZero(GetHealth()))
			{
				DamageInfo.Tag = NSTag::System::Death();
				MessageSystem.BroadcastMessage(DamageInfo.Tag, DamageInfo);
			}
		}
		else
		{
			DamageInfo.Tag = NSTag::System::Heal();
			MessageSystem.BroadcastMessage(DamageInfo.Tag, DamageInfo);
		}
	}
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


