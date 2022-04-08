// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MapVoteController.h"
#include "Engine/DataTable.h"
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
	DOREPLIFETIME(UMapVoteController, Voting_Votes);
}

void UMapVoteController::StartVoteForNextMap()
{
	GenerateMapsToVote();
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

	auto Rows = MapsTable->GetRowNames();
	if (Rows.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UMapVoteController says MapsTable is empty UMapVoteController"))
		return;
	}
	
	if (MapCount == 0)
	{
		Voting_Maps = Rows;
	}
	else
	{
		auto RowsNum = Rows.Num();
		for (uint8 i = 0; i < MapCount && i < RowsNum; i++)
		{
			auto RandomIndex = FMath::RandRange(0, Rows.Num()-1);
			Voting_Maps.Add(Rows[RandomIndex]);
			Rows.RemoveAt(RandomIndex);
		}
	}

	Voting_Votes.Reset(Voting_Maps.Num());

	UE_LOG(LogTemp, Warning, TEXT("Vote maps generated, array has %d num"), Voting_Maps.Num())
}

void UMapVoteController::VoteForMap(APlayerController* Player, FName MapRow, bool Up)
{
	if (VotedPlayers.Contains(Player)) return;

	if (Voting_Maps.Contains(MapRow))
	{
		VotedPlayers.Add(Player);
		Voting_Votes[Voting_Maps.Find(MapRow)]++;

		UE_LOG(LogTemp, Warning, TEXT("Map %s has %d votes"), *MapRow.ToString(), Voting_Votes[Voting_Maps.Find(MapRow)])
	}

	UE_LOG(LogTemp, Warning, TEXT("Player %s, Vote for map %s"), *Player->GetName(), *MapRow.ToString())
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
			break;
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
	}
	
	//return random leader map
	FName Leader = Leaders[FMath::RandRange(0, Leaders.Num())].Key;
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

bool UMapVoteController::VoteInProgress()
{
	return Voting_Maps.Num() > 0;
} 


void UMapVoteController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMapVoteController::OnRep_Voting_Maps()
{
	StartVoteForMap.Broadcast();
}

void UMapVoteController::OnRep_Voting_Votes()
{
	VotedForMap.Broadcast();
}