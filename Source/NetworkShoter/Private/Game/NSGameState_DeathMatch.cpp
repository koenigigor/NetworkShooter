// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState_DeathMatch.h"

#include "Net/UnrealNetwork.h"

void ANSGameState_DeathMatch::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSGameState_DeathMatch, KillCountLimit);
	DOREPLIFETIME(ANSGameState_DeathMatch, KillCount);
}

void ANSGameState_DeathMatch::CharacterKilled(APawn* WhoKilled)
{
	Super::CharacterKilled(WhoKilled);

	//increment kill count and check limits
	KillCount++;
	if (KillCount >= KillCountLimit)
	{
		if (auto GM = Cast<ANSGameMode>(AuthorityGameMode))
		{
			GM->EndMatch();
		}
	}
}
