// Fill out your copyright notice in the Description page of Project Settings.


#include "GMDeathMatch.h"

#include "Game/NSGameState_DeathMatch.h"

void AGMDeathMatch::InitGameState()
{
	Super::InitGameState();

	if (auto DeathMatchState = Cast<ANSGameState_DeathMatch>(GameState))
	{
		DeathMatchState -> KillCountLimit = KillCountLimit;
	}
}

void AGMDeathMatch::CharacterKilled(APawn* WhoKilled)
{
	Super::CharacterKilled(WhoKilled);

	//respawn after delay when he killed
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGMDeathMatch::RespawnDeathPlayer, RespawnDelay);
}

void AGMDeathMatch::RespawnDeathPlayer()
{
	if (HasMatchStarted() && DeathControllers.IsValidIndex(0))
	{
		SpawnPlayer(DeathControllers[0]);
		DeathControllers.RemoveAt(0);
	}
}
