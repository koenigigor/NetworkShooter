// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/TabMenu_Row.h"

#include "Game/NSPlayerState.h"

void UTabMenu_Row::NativeConstruct()
{
	Super::NativeConstruct();

	//check player if dead
	if (OwningPlayerState)
	{
		if (OwningPlayerState->bDeath)
		{
			BP_OnCharacterDead();
		}
		else
		{
			BP_OnCharacterRespawn();
		}
	} 
}

void UTabMenu_Row::Init(ANSPlayerState* PlayerState)
{
	
	if (!PlayerState || OwningPlayerState == PlayerState) { return; };
	if (OwningPlayerState)
	{
		//OwningPlayerState->PlayerStatisticUpdateDelegate.Remove();
	}
	
	OwningPlayerState = PlayerState;

	OwningPlayerState->PlayerStatisticUpdateDelegate.AddDynamic(this, &UTabMenu_Row::BP_OnStatisticUpdate);

	BP_OnStatisticUpdate();


	//bind to death/respawn
	OwningPlayerState->CharacterDeadDelegate.AddDynamic(this, &UTabMenu_Row::OnCharacterDeath);
	OwningPlayerState->CharacterRespawnDelegate.AddDynamic(this, &UTabMenu_Row::BP_OnCharacterRespawn);
}

void UTabMenu_Row::OnCharacterDeath(APawn* DeadPawn)
{
	BP_OnCharacterDead();
}

FPlayerStatistic UTabMenu_Row::GetPlayerStatistic()
{
	if (OwningPlayerState)
	{
		return OwningPlayerState->GetPlayerStatistic();
	}
	return  FPlayerStatistic();
}

FString UTabMenu_Row::GetPlayerName()
{
	return PlayerName = OwningPlayerState->GetPlayerName();
}
