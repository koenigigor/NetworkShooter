// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Components/MapVoteController.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogMapVoteController, All, All);

#pragma region FPlayersVote

void FPlayersVote::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const auto& Index : RemovedIndices)
	{
		auto& Vote = Votes[Index];
		ensure(Vote.Player);
		//UE_LOG(LogTemp, Warning, TEXT("OnRemove: Character = %s, Map = %s, bUp = %d"), (Vote.Player ? *Vote.Player->GetName() : *FString("None")), *Vote.Map.ToString(), Vote.bUp ? true : false);
		
		MapScoreAcceleration.FindOrAdd(Vote.Map) += Vote.bUp ? -1 : 1;
		VoteController->OnRemoveVote.Broadcast();
	}
}

void FPlayersVote::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const auto& Index : AddedIndices)
	{
		auto& Vote = Votes[Index];
		ensure(Vote.Player);
		//UE_LOG(LogTemp, Warning, TEXT("OnAdd: Character = %s, Map = %s, bUp = %d"), (Vote.Player ? *Vote.Player->GetName() : *FString("None")), *Vote.Map.ToString(), Vote.bUp ? true : false);

		MapScoreAcceleration.FindOrAdd(Vote.Map) += Vote.bUp ? 1 : -1;
		VoteController->OnAddVote.Broadcast(); 
	}
}

void FPlayersVote::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	//no entry
	for (const auto& Index : ChangedIndices)
	{
		auto& Vote = Votes[Index];
		UE_LOG(LogTemp, Error, TEXT("OnChange: Character = %s, Map = %s, bUp = %d"), (Vote.Player ? *Vote.Player->GetName() : *FString("None")), *Vote.Map.ToString(), Vote.bUp ? true : false);
		ensure(false); 
	}
}

void FPlayersVote::AddVote(APlayerState* Player, FName MapName, bool Up)
{
	if (!Player || MapName == NAME_None) return;

	auto& NewVote = Votes.Add_GetRef({Player, MapName, Up});
	MarkItemDirty(NewVote);

	MapScoreAcceleration.FindOrAdd(MapName) += Up ? 1 : -1;

	VoteController->OnAddVote.Broadcast();
}

void FPlayersVote::RemoveVote(APlayerState* Player)
{
	if (!Player) return;

	//remove last vote for specified player
	const auto LastVoteIndex = Votes.FindLastByPredicate([&](const FPlayersVoteEntry& Vote)
	{
		return Vote.Player == Player;
	});

	if (LastVoteIndex == INDEX_NONE) return;

	const auto VotedMap = Votes[LastVoteIndex].Map;
	MapScoreAcceleration[VotedMap] += Votes[LastVoteIndex].bUp ? -1 : 1;
	Votes.RemoveAt(LastVoteIndex);
	MarkArrayDirty();
}

void FPlayersVote::RemoveVote(APlayerState* Player, FName MapName)
{
	if (!Player || MapName == NAME_None) return;

	//remuve last vote
	const auto LastVoteIndex = Votes.FindLastByPredicate([&](const FPlayersVoteEntry& Vote)
	{
		return Vote.Player == Player && Vote.Map == MapName;
	});

	if (LastVoteIndex == INDEX_NONE) return;

	MapScoreAcceleration[MapName] += Votes[LastVoteIndex].bUp ? -1 : 1;
	Votes.RemoveAt(LastVoteIndex);
	MarkArrayDirty();

	VoteController->OnRemoveVote.Broadcast();

	/* //how iterate from end?
	for (auto It = Votes.CreateIterator(); It; ++It)
	{
		if (It->Player == Player && It->Map == MapName)
		{
			MapScoreAcceleration[MapName] += It->bUp ? -1 : 1;

			It.RemoveCurrent();
			MarkArrayDirty();
		}
	}
	*/
}

void FPlayersVote::Empty()
{
	Votes.Empty();
	MapScoreAcceleration.Empty();
}

FName FPlayersVote::GetLeader(const TArray<FName>& Maps) const
{
	//get maps with highest score
	TMap<FName, int32> Leaders;
	int32 MaxScore = -9999;
	for (auto MapVote : MapScoreAcceleration)
	{
		if (MapVote.Value > MaxScore)
		{
			Leaders.Empty();
			Leaders.Add(MapVote);
			MaxScore = MapVote.Value;
		}
		else if (MapVote.Value == MaxScore)
		{
			Leaders.Add(MapVote);
		}
	}

	//if no votes or negative score, return first 0 map
	if (MaxScore <= 0)
	{
		const auto ZeroMap = Maps.IndexOfByPredicate([&](const FName& Map)
		{
			return GetVotesFor(Map) == 0;
		});
		
		if (ZeroMap != INDEX_NONE) return VoteController->GetUsingMaps()[ZeroMap];
	}
	
	if (Leaders.Num() == 0) return NAME_None;						//no entry
	if (Leaders.Num() == 1) return Leaders.CreateIterator()->Key;	//get first (single) item 

	//return leader who get max votes first
	//iterate from back, until not get first winner
	for (int32 Index = Votes.Num() - 1; Index >= 0; --Index)
	{
		const auto& Vote = Votes[Index];

		if (Leaders.Contains(Vote.Map))
		{
			if (Vote.bUp)
			{
				//map been voted last, so she not winner
                Leaders.Remove(Vote.Map);
				if (Leaders.Num() == 1) return Leaders.CreateIterator()->Key;
			}
			else
			{
				//if last click down this map, so she was winner
				return Vote.Map;
			}
		}
	}

	
	checkNoEntry();
	return NAME_None;
}

int32 FPlayersVote::GetVotesFor(FName Map) const
{
	return MapScoreAcceleration.Contains(Map) ? MapScoreAcceleration[Map] : 0;
}

#pragma endregion FPlayersVote


UMapVoteController::UMapVoteController() : Votes(this)
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UMapVoteController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bVoteStarted);
	DOREPLIFETIME(ThisClass, StartVoteTime);
	DOREPLIFETIME(ThisClass, Duration);
	DOREPLIFETIME(ThisClass, UsingMaps);
	DOREPLIFETIME(ThisClass, Votes);
}

void UMapVoteController::StartVote()
{
	GenerateMapsToVote();

	bVoteStarted = true;
	StartVoteTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	GetWorld()->GetTimerManager().SetTimer(VoteTimerHandle, this, &UMapVoteController::FinishVote, Duration);
	OnStartVote.Broadcast();
}

void UMapVoteController::FinishVote()
{
	bVoteStarted = false;
	GetWorld()->GetTimerManager().ClearTimer(VoteTimerHandle);

	const auto Leader = MapsTable->FindRow<FMapInfo>(GetLeader(), "");
	ensure(Leader);
	OnVoteFinish.Broadcast(*Leader);
}

void UMapVoteController::OnStartVote_Client()
{
	OnStartVote.Broadcast();
}

void UMapVoteController::OnFinishVote_Client()
{
	const auto Leader = MapsTable->FindRow<FMapInfo>(GetLeader(), "");
	ensure(Leader);
	OnVoteFinish.Broadcast(*Leader);}

void UMapVoteController::GenerateMapsToVote()
{
	if (!MapsTable)
	{
		UE_LOG(LogMapVoteController, Error, TEXT("MapsTable is not set"))
		return;
	}
	UsingMaps.Empty();
	Votes.Empty();

	auto MapRows = MapsTable->GetRowNames();
	if (MapRows.Num() == 0)
	{
		UE_LOG(LogMapVoteController, Error, TEXT("MapsTable is empty"))
		return;
	}

	// Get all maps, or random count of them
	if (MapCount == 0 || MapCount > MapRows.Num())
	{
		UsingMaps = MapRows;
	}
	else
	{
		for (uint8 i = 0; i < MapCount; i++)
		{
			const auto RandomIndex = FMath::RandRange(0, MapRows.Num() - 1);
			UsingMaps.Add(MapRows[RandomIndex]);
			MapRows.RemoveAt(RandomIndex);
		}
	}
}

void UMapVoteController::VoteForMap(APlayerController* Player, FName MapRow, bool Up)
{
	//now re vote allowed
	if (!IsVoteInProgress()) return;

	UE_LOG(LogMapVoteController, Display, TEXT("Player %s, Vote for map %s"), *Player->GetName(), *MapRow.ToString())
	ensure(UsingMaps.Contains(MapRow));

	Votes.RemoveVote(Player->PlayerState);
	Votes.AddVote(Player->PlayerState, MapRow, Up);

	//finish if all player votes	//todo player state can be end for AI, (get count of player states? or AI vote for random map))
	const auto bAllPlayersVote = Votes.Num() >= GetWorld()->GetGameState()->PlayerArray.Num();
	if (bAllPlayersVote)
	{
		FinishVote();
	}
}

FName UMapVoteController::GetLeader() const
{
	return Votes.GetLeader(UsingMaps);
}

int32 UMapVoteController::GetVotesForMap(FName MapAssetName) const
{
	return Votes.GetVotesFor(MapAssetName);
}

bool UMapVoteController::IsVoteInProgress() const
{
	return bVoteStarted;
}

float UMapVoteController::GetVoteRemainingTime() const
{
	if (bVoteStarted)
	{
		const auto VoteTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - StartVoteTime;
		return Duration - VoteTime;
	}
	
	return 0.f;
}

void UMapVoteController::OnRep_Voting_Maps()
{
	if (bVoteStarted)
	{
		if (UsingMaps.Num() != 0)
		{
			OnStartVote_Client();
		}
	}
}

void UMapVoteController::OnRep_bVoteStarted()
{
	if (bVoteStarted)
	{
		if (UsingMaps.Num() != 0)
		{
			OnStartVote_Client();
		}
	}
	else
	{
		OnFinishVote_Client();
	}
}
