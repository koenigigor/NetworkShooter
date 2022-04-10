// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MapVoteController.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

UMapVoteController::UMapVoteController()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UMapVoteController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMapVoteController, Voting_Maps);
	DOREPLIFETIME_CONDITION_NOTIFY(UMapVoteController, Voting_Votes, COND_None, REPNOTIFY_Always); // arrays not rep notify if change inner property
}

void UMapVoteController::StartVote(float TimeToVote)
{
	GenerateMapsToVote();
	
	VoteTime=TimeToVote;	
	bVoteStarted = true;

	GetWorld()->GetTimerManager().SetTimer(VoteTimerHandle, this, &UMapVoteController::FinishVote, VoteTime);
}

void UMapVoteController::FinishVote()
{
	bVoteStarted=false;
	VoteTimerHandle.Invalidate();
	
	VoteFinish.Broadcast(GetLeader());
}

void UMapVoteController::GenerateMapsToVote()
{
	if (!MapsTable)
	{
		UE_LOG(LogTemp, Error, TEXT("MapsTable not set in UMapVoteController"))
		return;
	}
	Voting_Maps.Empty();
	VotedPlayers.Empty();
	Voting_Votes.Empty();

	auto Rows = MapsTable->GetRowNames();
	if (Rows.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UMapVoteController says MapsTable is empty UMapVoteController"))
		return;
	}

	// Get all maps, or random number
	if (MapCount == 0)
	{
		Voting_Maps = Rows;
		Voting_Votes.SetNum(Rows.Num());
	}
	else
	{
		auto RowsNum = Rows.Num();
		for (uint8 i = 0; i < MapCount && i < RowsNum; i++)
		{
			auto RandomIndex = FMath::RandRange(0, Rows.Num()-1);
			Voting_Maps.Add(Rows[RandomIndex]);
			Voting_Votes.Add(0);
			Rows.RemoveAt(RandomIndex);
		}
	}
}

void UMapVoteController::VoteForMap(APlayerController* Player, FName MapRow, bool Up)
{
	//if this player already voted, return
	if (VotedPlayers.Contains(Player)) return;
	
	if (Voting_Maps.Contains(MapRow))
	{
		VotedPlayers.Add(Player);
		Voting_Votes[Voting_Maps.Find(MapRow)]++;

		if (VotedPlayers.Num() >= GetWorld()->GetGameState()->PlayerArray.Num())
		{
			FinishVote();
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Player %s, Vote for map %s"), *Player->GetName(), *MapRow.ToString())
}

FName UMapVoteController::GetLeader()
{
    /** Array if we has multiple maps with same Vote count */
    TArray<TPair<FName, int32>> Leaders;

	for (int32 i = 0; i < Voting_Maps.Num(); i++)
	{
		if (!Leaders.IsValidIndex(0))
		{
			Leaders.Add(TPair<FName, int32>(Voting_Maps[i], Voting_Votes[i]));
		} else
		if (Voting_Votes[i] > Leaders[0].Value)
		{
			Leaders.Empty();
			Leaders.Add(TPair<FName, int32>(Voting_Maps[i], Voting_Votes[i]));
		} else
		if (Voting_Votes[i] == Leaders[0].Value)
		{
			Leaders.Add(TPair<FName, int32>(Voting_Maps[i], Voting_Votes[i]));
		}		
	}
	
	if (Leaders.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UMapVoteController::GetLeader array is empty"))
		return "None";
	}
	
	//return random leader map
	FName Leader = Leaders[FMath::RandRange(0, Leaders.Num()-1)].Key;
	return Leader;
}

int32 UMapVoteController::GetVotesForMap(FName MapAssetName)
{
	if (Voting_Maps.Contains(MapAssetName))
	{
		int32 MapIndex = Voting_Maps.Find(MapAssetName);
		return Voting_Votes[MapIndex];
	}

	return 0;
}

bool UMapVoteController::HasVoteStarted()
{
	return bVoteStarted;
}

float UMapVoteController::GetVoteRemainingTime()
{
	return GetWorld()->GetTimerManager().GetTimerRemaining(VoteTimerHandle);
}

void UMapVoteController::OnRep_Voting_Maps()
{
	if (bVoteStarted)
	{
		if (Voting_Maps.Num()!=0)
		{
			StartVoteForMap.Broadcast();
		}
	}
}

void UMapVoteController::OnRep_Voting_Votes()
{
	VotedForMap.Broadcast();
}

void UMapVoteController::OnRep_bVoteStarted()
{
	if (bVoteStarted)
	{
		if (Voting_Maps.Num()!=0)
		{
			StartVoteForMap.Broadcast();
		}
	}
	else
	{
		VoteFinish.Broadcast(GetLeader());
	}
}

void UMapVoteController::OnRep_VoteTime()
{
	GetWorld()->GetTimerManager().SetTimer(VoteTimerHandle, VoteTime, false);
}
