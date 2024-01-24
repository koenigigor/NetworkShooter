// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Components/TeamSetupManager.h"

#include "Game/NSGameState.h"
#include "Game/NSPlayerState.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogTeamSetupManager, All, All);

UTeamSetupManager::UTeamSetupManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTeamSetupManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTeamSetupManager, Teams, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(UTeamSetupManager, ChooseTeamDelay, COND_InitialOnly);
}

int32 UTeamSetupManager::GetMaxPlayersPerTeam() const
{
	const auto CountOfTeams = Teams.Num();
	if (CountOfTeams == 0) return 0;

	const auto PlayersCount = GetWorld()->GetGameState()->PlayerArray.Num();
	return FMath::DivideAndRoundUp(PlayersCount, CountOfTeams);
}

void UTeamSetupManager::ShufflePlayers(bool bReset)
{
	const int32 PlayersPerTeam = GetMaxPlayersPerTeam();

	TArray<ANSPlayerState*> PlayersToShuffle;
	GetPlayersToShuffle(PlayersToShuffle, bReset);

	TMap<uint8, int32> TeamCountMap;
	GetTeamAndCount(TeamCountMap, !bReset);

	//shuffle teams
	for (const auto Player : PlayersToShuffle)
	{
		const auto TeamToAdd = GetRandomFreeTeam(TeamCountMap, PlayersPerTeam);

		TeamCountMap[TeamToAdd]++;

		Player->SetGenericTeamId(FGenericTeamId(TeamToAdd));
	}
}

void UTeamSetupManager::AddPlayerInTeam_Implementation(ANSPlayerState* Player, EGameTeam PreferTeam)
{
	ClearIfWaitingChoose(Player);

	if (!Teams.Contains(PreferTeam))
		PreferTeam = Teams[0];

	const auto PlayersPerTeam = GetMaxPlayersPerTeam();

	TMap<uint8, int32> TeamCountMap;
	GetTeamAndCount(TeamCountMap, true);

	const uint8 TeamToAdd = TeamCountMap[uint8(PreferTeam)] < PlayersPerTeam
		                        ? uint8(PreferTeam)
		                        : GetRandomFreeTeam(TeamCountMap, PlayersPerTeam);

	Player->SetGenericTeamId(FGenericTeamId(TeamToAdd));
}

bool UTeamSetupManager::AddPlayerInTeam_Validate(ANSPlayerState* Player, EGameTeam PreferTeam)
{
	return true;
}

void UTeamSetupManager::WaitUntilPlayerChooseTeam(APlayerController* Player)
{
	const auto PlayerState = Player->GetPlayerState<ANSPlayerState>();

	if (!PlayerState)
	{
		UE_LOG(LogTeamSetupManager, Display, TEXT("UTeamSetupManager::WaitUntilPlayerChooseTeam PlayerState is not a ANSPlayerState"))
	}

	FTimerHandle WaitTimerHandle;
	WaitChooseTeamTimers.Add(PlayerState, WaitTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, this, &UTeamSetupManager::OnChooseTimeEnd, ChooseTeamDelay);
}

void UTeamSetupManager::OnChooseTimeEnd()
{
	//get player who ends time
	//map clears inside AddPlayerInTeam
	TArray<ANSPlayerState*> EndWaiting;
	for (const auto& Waiting : WaitChooseTeamTimers)
	{
		const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(Waiting.Value);
		if (RemainingTime <= 0)
		{
			EndWaiting.Add(Waiting.Key);
		}
	}

	//add players in random team
	for (const auto& Player : EndWaiting)
	{
		AddPlayerInTeam(Player, Teams[FMath::RandRange(0, Teams.Num() - 1)]);
	}
}

void UTeamSetupManager::ClearIfWaitingChoose(ANSPlayerState* Player)
{
	if (!WaitChooseTeamTimers.Contains(Player)) return;

	auto TimerHandel = WaitChooseTeamTimers.FindAndRemoveChecked(Player);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandel);

	if (const auto Controller = Player->GetOwner<APlayerController>())
		PlayerChooseTeamAfterWait.Broadcast(Controller);
}

uint8 UTeamSetupManager::GetRandomFreeTeam(const TMap<uint8, int32>& TeamCountMap, const int32 PlayersPerTeam)
{
	//GetRandom team id, if no get random team from free slot ones
	uint8 FreeTeam = uint8(Teams[FMath::RandRange(0, Teams.Num() - 1)]);
	if (TeamCountMap[FreeTeam] >= PlayersPerTeam)
	{
		//get team with free slots
		const auto FreeTeams = Teams.FilterByPredicate([&](const EGameTeam& Team)
		{
			return TeamCountMap[uint8(Team)] < PlayersPerTeam;
		});

		check(FreeTeams.Num() != 0);

		//get random of them
		FreeTeam = uint8(FreeTeams[FMath::RandRange(0, FreeTeams.Num() - 1)]);
	}

	return FreeTeam;
}


TArray<ANSPlayerState*> UTeamSetupManager::GetTeam(uint8 TeamIndex)
{
	TArray<ANSPlayerState*> Output;

	for (auto& Player : GetWorld()->GetGameState()->PlayerArray)
	{
		auto NSPlayer = StaticCast<ANSPlayerState*>(Player);
		if (NSPlayer->GetGenericTeamId().GetId() == TeamIndex)
			Output.Add(NSPlayer);
	}

	return Output;
}

void UTeamSetupManager::GetNextPlayerInTeam(uint8 TeamIndex, ANSPlayerState*& NextPlayerInTeam, bool bNext, bool bLifePlayer)
{
	auto Team = GetTeam(TeamIndex);

	if (Team.Num() == 0)
	{
		NextPlayerInTeam = nullptr;
		return;
	}

	const int32 StartSearchIndex = NextPlayerInTeam ? Team.IndexOfByKey(NextPlayerInTeam) : INDEX_NONE;

	int32 NextIndex = StartSearchIndex;
	for (int32 IterateCount = 0; IterateCount < Team.Num(); ++IterateCount)
	{
		NextIndex = bNext ? NextIndex + 1 : NextIndex - 1;
		if (NextIndex > Team.Num() - 1) { NextIndex = 0; } //no FMath::Wrap because INDEX_NONE - 1  = -2 
		if (NextIndex < 0) { NextIndex = Team.Num() - 1; }

		if (!bLifePlayer || Team[NextIndex]->IsLife())
		{
			NextPlayerInTeam = Team[NextIndex];
			return;
		}
	}

	NextPlayerInTeam = nullptr;
	return;
}

void UTeamSetupManager::GetTeamAndCount(TMap<uint8, int32>& TeamCountMap, const bool CheckIfAlreadyInTeam)
{
	TeamCountMap.Empty();

	//create keys for used teams, for protect using this map outside
	for (auto& Team : Teams)
	{
		TeamCountMap.Add(uint8(Team), 0);
	}

	if (!CheckIfAlreadyInTeam) { return; }

	auto PlayerArray = GetWorld()->GetGameState()->PlayerArray;
	for (const auto& Player : PlayerArray)
	{
		auto TeamId = Cast<IGenericTeamAgentInterface>(Player)->GetGenericTeamId().GetId();
		if (TeamCountMap.Contains(TeamId))
		{
			TeamCountMap[TeamId]++;
		}
		else
		{
			TeamCountMap.Add(TeamId, 1);
		}
	}
}

void UTeamSetupManager::GetPlayersToShuffle(TArray<ANSPlayerState*>& PlayersToShuffle, bool AddAlreadyInTeam)
{
	PlayersToShuffle.Empty();

	auto PlayerArray = GetWorld()->GetGameState()->PlayerArray;

	for (auto& Player : PlayerArray)
	{
		auto NSPlayer = Cast<ANSPlayerState>(Player);
		if (AddAlreadyInTeam)
		{
			PlayersToShuffle.Add(NSPlayer);
		}
		else
		{
			if (!Teams.Contains(NSPlayer->GetTeamID()))
			{
				PlayersToShuffle.Add(NSPlayer);
			}
		}
	}
}
