// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "Net/UnrealNetwork.h"

void ANSGameState::AddPlayerPawn(APawn* Pawn)
{
	//get team name TODO
	FName TeamName = "Team1";

	//add pawn in team array
	TArray<APawn*>* TeamListPtr = nullptr;
	//auto TeamList =
	GetTeamList(TeamName, TeamListPtr);

	if (!TeamListPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("TeamListPtr is null"))
		return;
	}
	
	TeamListPtr -> AddUnique(Pawn);

	//todo actually add in array? or it copy fo array

	
	/** Test **/
	UE_LOG(LogTemp, Warning, TEXT("Added: Team1 Count: %d"), Team1.Num())
}

void ANSGameState::RemovePawn(APawn* Pawn)
{
	//get team name TODO
	FName TeamName = "Team1";

	
	//remove pawn from team array
	
	TArray<APawn*>* TeamListPtr = nullptr;
	GetTeamList(TeamName, TeamListPtr);
	if (TeamListPtr)
		TeamListPtr -> Remove(Pawn);

	
	//client rep notify on removed (for swap spectator)

	/** Test **/
	UE_LOG(LogTemp, Warning, TEXT("Removed: TeamList Count: %d"), TeamListPtr -> Num())
}

void ANSGameState::GetNextActorInTeam(FName Team, AActor*& NextActorInTeam, int32& NumberInTeam, bool bNext)
{
	UE_LOG(LogTemp, Warning, TEXT("ANSGameState::GetNextActorInTeam started"))

	//Get Team List for target team
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

	//on call, NextActor can give us previous selected actor
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
				UE_LOG(LogTemp, Warning, TEXT("ANSGameState::GetNextActorInTeam NextActor founded by index : %d"), NumberInTeam)
				return;
			}
			else
			{
				int32 PreviousIndex = (PreviousActorIndex-1 >= 0) ? PreviousActorIndex-1 : TeamListPtr->Num()-1;
				
				NextActorInTeam = (*TeamListPtr)[PreviousIndex];
				NumberInTeam = PreviousIndex;
				UE_LOG(LogTemp, Warning, TEXT("ANSGameState::GetNextActorInTeam NextActor founded by index : %d"), NumberInTeam)
				return;
			}
		}
	}
	else
	{
		//return first element in team
		NextActorInTeam = (*TeamListPtr)[0];
		NumberInTeam = 0;
		UE_LOG(LogTemp, Warning, TEXT("ANSGameState::GetNextActorInTeam no next actor, get 0 element : %d"), NumberInTeam)
		return;
	}
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

void ANSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSGameState, Team1);
	DOREPLIFETIME(ANSGameState, Team2);
}
