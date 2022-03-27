// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GMDeathMatch.h"

#include "Game/NSGameState_DeathMatch.h"

void AGMDeathMatch::InitGameState()
{
	Super::InitGameState();

	if (auto DeathMatchState = Cast<ANSGameState_DeathMatch>(GameState))
	{
		DeathMatchState -> KillCountLimit = KillCountLimit;
	}
}