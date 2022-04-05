// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GMTeamMatch.h"

#include "Game/NSPlayerState.h"
#include "GameFramework/GameStateBase.h"

void AGMTeamMatch::ShuffleTeam()
{
	auto CountOfTeams = Teams.Num();

	if (CountOfTeams == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AGMTeamMatch::ShuffleTeam Teams not setup"))
		return;
	}

	auto PlayerArray = GameState -> PlayerArray;
	auto PlayersCount = PlayerArray.Num();
	int32 PlayersPerTeam = PlayersCount / CountOfTeams;

	//if Players count not even, add 1 player slot
	if (PlayersCount % CountOfTeams != 0)
		PlayersPerTeam++;
	
	//team overflow protection
	TMap<int32, int32> TeamProtection;  //<Team index in Teams setup araay, CountInTeam>
	for (int32 i = 0; i<CountOfTeams; i++)
	{
		TeamProtection.Add(i, 0);
	}

	//shuffle teams
	for (const auto& Player : PlayerArray)
	{
		auto NSPlayer = Cast<ANSPlayerState>(Player);

		//Get index team to add
		int32 TeamIndex;
		do {
			TeamIndex = FMath::RandRange(0, CountOfTeams-1);
		} while (TeamProtection[TeamIndex] >= PlayersPerTeam);

		//add player in team
		TeamProtection[TeamIndex]++;

		uint8 TeamId = uint8(Teams[TeamIndex]);
		NSPlayer->SetGenericTeamId(FGenericTeamId(TeamId));
	}

}
