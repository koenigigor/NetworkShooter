// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/NSPlayerStart.h"

#include "Game/NSPlayerState.h"

bool ANSPlayerStart::CanSpawn(AController* Controller)
{
	auto TeamInterface = Cast<IGenericTeamAgentInterface>(Controller);
	if (!TeamInterface)
	 TeamInterface = Cast<IGenericTeamAgentInterface>(Controller -> PlayerState);

	if (!TeamInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnedTarget %s has no team interface, spawn allowed"), *Controller->GetName())
		return true;
	}

	uint8 WantSpawnTeam = TeamInterface->GetGenericTeamId().GetId();

	auto bSpawnAllowed = false; //AllowedTeams.Contains(WantSpawnTeam);
	for (const auto& Team : AllowedTeams)
	{
		if (uint8(Team) == WantSpawnTeam)
		{
			bSpawnAllowed = true;
			break;
		}
	} 
	
	return bSpawnAllowed;
}
