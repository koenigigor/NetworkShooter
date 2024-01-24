// Fill out your copyright notice in the Description page of Project Settings.


#include "NSPlayerStart.h"

#include "Game/NSPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerStart, All, All);

bool ANSPlayerStart::CanSpawn(AController* Controller) const
{
	auto TeamInterface = Cast<IGenericTeamAgentInterface>(Controller);
	if (!TeamInterface)
		TeamInterface = Cast<IGenericTeamAgentInterface>(Controller->PlayerState);

	if (!TeamInterface)
	{
		UE_LOG(LogPlayerStart, Warning, TEXT("SpawnedTarget %s has no implement IGenericTeamAgentInterface, spawn allowed"), *Controller->GetName())
		return true;
	}

	const uint8 WantSpawnTeam = TeamInterface->GetGenericTeamId().GetId();

	const bool bTeamAllowed = AllowedTeams.ContainsByPredicate([&](const EGameTeam& Team)
	{
		return uint8(Team) == WantSpawnTeam;
	});

	return bTeamAllowed;
}
