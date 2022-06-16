// Fill out your copyright notice in the Description page of Project Settings.


#include "NSStructures.h"

#include "GameFramework/PlayerState.h"
#include "Interface/DamageDescriptionInterface.h"


FPlayerStatistic& FPlayerStatistic::operator+=(const FPlayerStatistic& Other){

	this->AssistCount += Other.AssistCount;
	this->KillCount += Other.KillCount;
	this->DeathCount += Other.DeathCount;
	
	return *this;
}


FDamageInfo::FDamageInfo(APlayerState* InInstigatorState, AActor* InCauser, AActor* InTarget, float InDamage, const UGameplayAbility* InSourceAbilityCDO)
	:InstigatorState(InInstigatorState), DamageCauser(InCauser), Target(InTarget), Damage(InDamage), SourceAbilityCDO(InSourceAbilityCDO)
{
}

FString FDamageInfo::GetMessage() const
{
	UE_LOG(LogTemp, Display, TEXT("FDamageInfo::GetMessage Search in SourceAbilityCDO"))
	
	if (SourceAbilityCDO)
	{
		if (const auto DescriptionInterface = Cast<IDamageDescriptionInterface>(SourceAbilityCDO))
		{
			return DescriptionInterface->GetDamageDescription(*this);
		}
	}

	UE_LOG(LogTemp, Display, TEXT("FDamageInfo::GetMessage Search in causeer"))

	if (DamageCauser)
	{
		if (const auto DescriptionInterface = Cast<IDamageDescriptionInterface>(DamageCauser))
		{
			return DescriptionInterface->GetDamageDescription(*this);
		}
	}

	UE_LOG(LogTemp, Display, TEXT("FDamageInfo::GetMessage Generate default message"))

	FString InstigatorName;
	FString TargetName;
	FString DamageCauserName;
	
	if (InstigatorState)
	{
		InstigatorName = InstigatorState -> GetPlayerName();
	}
	
	if (Target)
	{
		TargetName = Target->GetInstigatorController()->PlayerState->GetPlayerName();
	}
	
	if (DamageCauser) //first spawned actor on equipped weapon
	{
		DamageCauserName = DamageCauser->GetName();
	}
		
	return FString::Printf(TEXT("%s receive %f damage from %s instigated by %s"), *TargetName, Damage, *DamageCauserName, *InstigatorName);
}
