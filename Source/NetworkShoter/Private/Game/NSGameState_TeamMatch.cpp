// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState_TeamMatch.h"

#include "Net/UnrealNetwork.h"

void ANSGameState_TeamMatch::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSGameState_TeamMatch, Team1);
}

void ANSGameState_TeamMatch::GetTeamList(int32 TeamIndex, TArray<ANSPlayerState*>*& TeamListPtr)
{
	Super::GetTeamList(TeamIndex, TeamListPtr);

	if (TeamIndex == 1)
	{
		TeamListPtr = &Team1;
		return;
	}
}

void ANSGameState_TeamMatch::OnRep_Team1()
{
}
