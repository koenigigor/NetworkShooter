// Fill out your copyright notice in the Description page of Project Settings.


#include "NSPlayerStart.h"

#include "Game/NSPlayerState.h"

bool ANSPlayerStart::CanSpawn(AController* Controller)
{
	//Get team name from controller
	int32 PlayerTeamIndex = -1;
	if (auto NSPlayerState = Controller->GetPlayerState<ANSPlayerState>())
		PlayerTeamIndex = NSPlayerState->TeamIndex;
	
	if (PlayerTeamIndex == -1 || TeamIndexes.Contains(PlayerTeamIndex))
		return true;

	return false;
	
	/** overlap tested in GameMode::ChoosePlayerStart */
}
