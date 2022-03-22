// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/TabMenu_Row.h"

#include "NSPlayerState.h"

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
	//TODO
}

void UTabMenu_Row::OnPawnDeath(ANSPlayerState* PlayerState)
{
	SetIsEnabled(false);
}

void UTabMenu_Row::OnPawnSpawn(ANSPlayerState* PlayerState)
{
	SetIsEnabled(true);
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
