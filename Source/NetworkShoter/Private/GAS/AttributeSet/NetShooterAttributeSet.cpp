// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/NetShooterAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "NSStructures.h"
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
//move in pre change, for keep raw damage value (if use EGameplayModOp::Override, cant detect previous value and calculate damage)
bool UNetShooterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) 
{
	Super::PreGameplayEffectExecute(Data);
//[Server]	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const auto Causer = Data.EffectSpec.GetEffectContext().GetEffectCauser();
		const auto Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();
		const auto Target = GetOwningActor();
		const auto Damage = Data.EvaluatedData.ModifierOp == EGameplayModOp::Override ? Data.EvaluatedData.Magnitude - GetHealth() : Data.EvaluatedData.Magnitude;
		const auto Ability = Data.EffectSpec.GetEffectContext().GetAbility();
			
		ensure(Causer);

		const auto InstigatorState = Cast<APlayerState>(Instigator);
		if (!InstigatorState)
		{
			UE_LOG(LogTemp, Display, TEXT("Intend instigator is PlayerState, in %s"), Ability ? *Ability->GetName() : *Causer->GetName())
		}
        			
		FDamageInfo DamageInfo(InstigatorState, Causer, Target, Damage, Ability);

		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		
		if (Damage < 0.f)
		{			
			DamageInfo.Tag = TAG_MESSAGE_SYSTEM_DAMAGE;
			MessageSystem.BroadcastMessage(DamageInfo.Tag, DamageInfo);
			
			if (GetHealth() + Damage <= 0 && !bOutOfHealth)
			{
				bOutOfHealth = true;
				DamageInfo.Tag = TAG_MESSAGE_SYSTEM_DEATH;
				MessageSystem.BroadcastMessage(DamageInfo.Tag, DamageInfo);
			}
			
			//GC_HitReaction
			{
				FGameplayCueParameters CueParameters = FGameplayCueParameters(Data.EffectSpec.GetEffectContext());
				CueParameters.RawMagnitude = Damage; //damage magnitude
				CueParameters.Normal = Target && Instigator ? (Target->GetActorLocation() - Instigator->GetActorLocation()).GetSafeNormal() : FVector::ZeroVector; //enemy direction
			
				if (const auto HitPtr = Data.EffectSpec.GetEffectContext().GetHitResult())
				{
					CueParameters.PhysicalMaterial = HitPtr->PhysMaterial;
				}
				else
				{
					UE_LOG(LogTemp, Display, TEXT("UNetShooterAttributeSet::PostGameplayEffectExecute, Data.EffectSpec.GetEffectContext().GetHitResult() is null"))
				}

				GetOwningAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.HitReaction"), CueParameters);
			}
		}
		else
		{
			DamageInfo.Tag = TAG_MESSAGE_SYSTEM_HEAL;
			MessageSystem.BroadcastMessage(DamageInfo.Tag, DamageInfo);
		}
	}

	return Super::PreGameplayEffectExecute(Data);
}

void UNetShooterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		bOutOfHealth = GetHealth() <= 0.f;
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


