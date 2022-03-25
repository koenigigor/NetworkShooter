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
	for (int32 i = 1; i<=CountOfTeams; i++)
	{
		TeamProtection.Add(i, 0);
	}

	//shuffle teams
	for (const auto& Player : PlayerArray)
	{
		auto NSPlayer = Cast<ANSPlayerState>(Player);

		//Get number team to add
		int32 TeamNumber;
		do {
			TeamNumber = FMath::RandRange(int32(1), CountOfTeams);
			UE_LOG(LogTemp, Error, TEXT("Test team number : %d, Team count : %d, Players per team : %d"), TeamNumber, TeamProtection[TeamNumber], PlayersPerTeam)
		} while (TeamProtection[TeamNumber] >= PlayersPerTeam);

		//add player in team
		TeamProtection[TeamNumber]++;
			
		if (TeamNumber == 1)
			NSPlayer->Team = "Team 1";

		if (TeamNumber == 2)
			NSPlayer->Team = "Team 2";
	} 
}
