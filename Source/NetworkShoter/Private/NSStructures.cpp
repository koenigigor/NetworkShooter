// Fill out your copyright notice in the Description page of Project Settings.


#include "NSStructures.h"

#include "GameFramework/PlayerState.h"
#include "Interface/DamageDescriptionInterface.h"

DEFINE_LOG_CATEGORY_STATIC(LogDamageInfo, All, All);

UE_DEFINE_GAMEPLAY_TAG(TAG_MESSAGE_SYSTEM_DAMAGE, "Message.System.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_MESSAGE_SYSTEM_HEAL, "Message.System.Heal");
UE_DEFINE_GAMEPLAY_TAG(TAG_MESSAGE_SYSTEM_DEATH, "Message.System.Death");
UE_DEFINE_GAMEPLAY_TAG(TAG_CHAT_GENERAL, "Chat.General");
UE_DEFINE_GAMEPLAY_TAG(TAG_CHAT_GROUP, "Chat.Group");
UE_DEFINE_GAMEPLAY_TAG(TAG_CHAT_LOCATION, "Chat.Location");

FPlayerStatistic& FPlayerStatistic::operator+=(const FPlayerStatistic& Other)
{
	this->AssistCount += Other.AssistCount;
	this->KillCount += Other.KillCount;
	this->DeathCount += Other.DeathCount;

	return *this;
}


FDamageInfo::FDamageInfo()
{
}

FDamageInfo::FDamageInfo(APlayerState* InInstigatorState, AActor* InCauser, AActor* InTarget, float InDamage, const UGameplayAbility* InSourceAbilityCDO)
	: InstigatorState(InInstigatorState), DamageCauser(InCauser), Target(InTarget), Damage(InDamage), SourceAbilityCDO(InSourceAbilityCDO)
{
}

FString FDamageInfo::GetMessage() const
{
	UE_LOG(LogDamageInfo, Display, TEXT("Generate damage message..."));

	if (SourceAbilityCDO)
	{
		if (SourceAbilityCDO->Implements<UDamageDescriptionInterface>())
		{
			UE_LOG(LogDamageInfo, Display, TEXT("... found in source ability"));
			return IDamageDescriptionInterface::Execute_GetDamageDescription(SourceAbilityCDO, *this);
		}
	}

	if (DamageCauser)
	{
		if (DamageCauser->Implements<UDamageDescriptionInterface>())
		{
			UE_LOG(LogDamageInfo, Display, TEXT("... found in damage causer"));
			return IDamageDescriptionInterface::Execute_GetDamageDescription(DamageCauser, *this);
		}
	}

	UE_LOG(LogDamageInfo, Display, TEXT("... generate default message ..."));

	FString InstigatorName = "???";
	FString TargetName = "???";
	FString DamageCauserName = "???";

	if (InstigatorState)
	{
		InstigatorName = InstigatorState->GetPlayerName();
	}

	if (Target)
	{	//todo crash on death
		//TargetName = Target->GetInstigator()->GetPlayerState()->GetPlayerName();
	}

	if (DamageCauser) //first spawned actor from equipped weapon
	{
		DamageCauserName = DamageCauser->GetName();
	}
	UE_LOG(LogDamageInfo, Display, TEXT("... default message generated"));
	return FString::Printf(TEXT("%s receive %f damage from %s instigated by %s"), *TargetName, Damage, *DamageCauserName, *InstigatorName);
}

bool FDamageInfo::IsValid() const
{
	return InstigatorState ? true : false;
}
