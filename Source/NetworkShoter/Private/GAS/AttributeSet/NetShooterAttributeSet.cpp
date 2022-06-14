// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/NetShooterAttributeSet.h"

#include "GameplayEffectExtension.h"
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

	//TODO TODO TODO damage notify there
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (Data.EvaluatedData.Magnitude < 0.f)	//is damage
		{
			const auto Causer = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			const auto Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();
			const auto Target = GetOwningActor();
			const auto Damage = Data.EvaluatedData.Magnitude;
			
			ensure(Causer);
			ensure(Instigator);
			
			UE_LOG(LogTemp, Display, TEXT("UNetShooterAttributeSet %s damaged from %s by %s on %f"), *Target->GetName(), *Instigator->GetName(), *Causer->GetName(), Damage)
		
			//GetWorld() -> GetGameState<ANSGameState>() -> ApplyDamageInfoFromActors(OwnerActor->GetInstigatorController(), DamagedActor, DamageCauser, Damage);

			//is death
			if (FMath::IsNearlyZero(GetHealth()))
			{
				
			}
			
			/* Standard damage notify
			const auto DamageTypeCDO = GetDefault<UDamageType>();
			DamagedActor->ReceiveAnyDamage(Damage, DamageTypeCDO, OwnerActor->GetInstigatorController(), DamageCauser);
		DamagedActor->OnTakeAnyDamage.Broadcast(DamagedActor, Damage, DamageTypeCDO, OwnerActor->GetInstigatorController(), DamageCauser);
			if (OwnerActor->GetInstigatorController() != nullptr)
			{
				OwnerActor->GetInstigatorController()->InstigatedAnyDamage(Damage, DamageTypeCDO, DamagedActor, DamageCauser);
			}
			*/				
		}
		else    //is healing
		{
			UE_LOG(LogTemp, Display, TEXT("UNetShooterAttributeSet Im Healed on %f"), Data.EvaluatedData.Magnitude)
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


