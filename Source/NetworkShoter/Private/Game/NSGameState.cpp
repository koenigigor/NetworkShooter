// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "Net/UnrealNetwork.h"


void ANSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSGameState, Team1);
	DOREPLIFETIME(ANSGameState, Team2);
}


void ANSGameState::AddPlayerPawn(APawn* Pawn)
{
	//TODO Get team
	FName TeamName = "Team1";

	//Get Team Array
	TArray<APawn*>* TeamListPtr = nullptr;
	GetTeamList(TeamName, TeamListPtr);
	if (!TeamListPtr) { return; }
	
	TeamListPtr -> AddUnique(Pawn);
}

void ANSGameState::RemovePawn(APawn* Pawn)
{
	//TODO Get team
	FName TeamName = "Team1";

	
	//remove pawn from team array
	TArray<APawn*>* TeamListPtr = nullptr;
	GetTeamList(TeamName, TeamListPtr);
	
	if (TeamListPtr)
		TeamListPtr -> Remove(Pawn);
}

void ANSGameState::GetNextActorInTeam(FName Team, AActor*& NextActorInTeam, int32& NumberInTeam, bool bNext)
{
	//Get Team array
	TArray<APawn*>* TeamListPtr = nullptr;
	GetTeamList(Team, TeamListPtr);
	if (!ensure(TeamListPtr)) { return; }

	//if Empty return failed result
	if (TeamListPtr -> Num() <= 0)
	{
		NextActorInTeam = nullptr;
		NumberInTeam = -1;
		return;
	}

	//if Previous actor or index was been set
	auto PreviousActorInTeam = NextActorInTeam;
	if (PreviousActorInTeam || (NumberInTeam>-1))
	{
		//Get his index in array
		int32 PreviousActorIndex = -1;
		if (PreviousActorInTeam)
		{
			for (int32 i = 0; i < TeamListPtr->Num(); i++)
			{	
				if ((*TeamListPtr)[i] == PreviousActorInTeam)
				{
					PreviousActorIndex = i;
					break;
				}
			}
		}
		
		//if previous actor not founded, check index
		if (PreviousActorIndex == -1)
		{
			PreviousActorIndex = TeamListPtr->IsValidIndex(NumberInTeam) ? NumberInTeam : -1;
			//todo if dead character give self number, maybe need do -1, and protect 0 -> -1
		}

		
		//if index successfully founded founded
		if (PreviousActorIndex > -1)
		{
			//return next or previous element
			if (bNext)
			{
				int32 NextIndex = (PreviousActorIndex+1 < TeamListPtr->Num()) ? PreviousActorIndex+1 : 0;
				
				NextActorInTeam = (*TeamListPtr)[NextIndex];
				NumberInTeam = NextIndex;
				return;
			}
			else
			{
				int32 PreviousIndex = (PreviousActorIndex-1 >= 0) ? PreviousActorIndex-1 : TeamListPtr->Num()-1;
				
				NextActorInTeam = (*TeamListPtr)[PreviousIndex];
				NumberInTeam = PreviousIndex;
				return;
			}
		}
	}

	//if not set or not find, return 0 element
	NextActorInTeam = (*TeamListPtr)[0];
	NumberInTeam = 0;
	return;
}

void ANSGameState::GetTeamList(FName Team, TArray<APawn*>*& TeamListPtr)
{
	if (Team == "Team1")
	{
		TeamListPtr = &Team1;
		return;
	}
	if (Team == "Team2")
	{
		TeamListPtr = &Team2;
		return;
	}
}

void ANSGameState::OnRep_Team1()
{
}

void ANSGameState::OnRep_Team2()
{
}

