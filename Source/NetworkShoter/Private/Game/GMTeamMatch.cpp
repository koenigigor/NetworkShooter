// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GMTeamMatch.h"

#include "Game/NSPlayerState.h"
#include "GameFramework/GameStateBase.h"

void AGMTeamMatch::ShuffleTeam()
{
	int32 CountOfTeams = 2; //if we want 3 or 4 teams

	auto PlayerArray = GameState -> PlayerArray;
	auto PlayersCount = PlayerArray.Num();
	int32 PlayersPerTeam = PlayersCount / CountOfTeams;

	//if Players count not even, add 1 player slot
	if (PlayersCount % CountOfTeams != 0)
		PlayersPerTeam++;	

	//team overflow protection
	TMap<int32, int32> TeamProtection;
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
			TeamIndex = FMath::RandRange(int32(0), CountOfTeams-1);
			UE_LOG(LogTemp, Error, TEXT("Test team number : %d, Team count : %d, Players per team : %d"), TeamIndex, TeamProtection[TeamIndex], PlayersPerTeam)
		} while (TeamProtection[TeamIndex] >= PlayersPerTeam);

		//add player in team
		TeamProtection[TeamIndex]++;

		NSPlayer->TeamIndex = TeamIndex;

		//AddPlayerInTeamList(Cast<AController>(NSPlayer->GetOwner()));
	}

	//get game state, refresh team lists
}
