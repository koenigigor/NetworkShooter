// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "Game/NSGameMode.h"
#include "Game/NSPlayerState.h"
#include "Game/Components/ChatController.h"
#include "Game/Components/DamageHistoryComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


ANSGameState::ANSGameState()
{
	SetActorTickEnabled(false);

	DamageHistory = CreateDefaultSubobject<UDamageHistoryComponent>(TEXT("DamageHistory"));
}

void ANSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANSGameState, MatchTimeLimit);
	DOREPLIFETIME(ANSGameState, MatchState);
	DOREPLIFETIME(ANSGameState, bMatchTimeLimit);
	DOREPLIFETIME(ANSGameState, bFriendlyFire);
	DOREPLIFETIME(ANSGameState, MatchTime);
	DOREPLIFETIME_CONDITION(ANSGameState, WaitStartMatchTime, COND_InitialOnly);
}

void ANSGameState::BeginPlay()
{
	Super::BeginPlay();

	if (GetMatchState() == EMatchState::WaitingConnection || GetMatchState() == EMatchState::WaitingToStart)
		MatchTime = WaitStartMatchTime;

	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ThisClass::TickMatchTime, 1.f, true, 0.f);
}

void ANSGameState::TickMatchTime()
{
	if (MatchState == EMatchState::WaitingToStart)
	{
		MatchTime--;
		
		if (MatchTime<=0 && HasAuthority())
		{
			Cast<ANSGameMode>(GetWorld()->GetAuthGameMode())->StartMatch();
		}
	}
	else if	(MatchState == EMatchState::InProgress)
	{
		MatchTime++;
		
		if (bMatchTimeLimit && HasAuthority() && MatchTime >= MatchTimeLimit.GetTotalSeconds())
		{
			Cast<ANSGameMode>(GetWorld()->GetAuthGameMode())->EndMatch();
		}
	}
}

void ANSGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	PlayerAddedDelegate.Broadcast(PlayerState);
}

void ANSGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	PlayerRemovedDelegate.Broadcast(PlayerState);
}


//~==============================================================================================
// Match State

void ANSGameState::WaitingToStartMatchHandle_Implementation()
{
	MatchTime = WaitStartMatchTime;
	WaitingToStartMatch.Broadcast();
}

void ANSGameState::StartMatchHandle_Implementation()
{
	BP_MatchStarted();
	MatchStartDelegate.Broadcast();
	
	MatchStartTime = GetWorld()->GetTimeSeconds();
	MatchTime = 0.f;
}

void ANSGameState::EndMatchHandle_Implementation()
{	
	BP_MatchFinished();
	MatchEndDelegate.Broadcast();
}

bool ANSGameState::HasMatchStarted() const
{
	//return Super::HasMatchStarted();
	return MatchState == EMatchState::InProgress;
}


//~==============================================================================================
// Match Statistic

bool ANSGameState::CanBeDamaged(const AActor* Target, const AActor* DamageInstigator) const 
{
	const auto Attitude = FGenericTeamId::GetAttitude(Target, DamageInstigator);
	
	if (bFriendlyFire && Attitude == ETeamAttitude::Friendly)
	{
		return false;
	}
	return true;
}


//~==============================================================================================
// Team List

TArray<ANSPlayerState*> ANSGameState::GetTeam(uint8 TeamIndex) const
{
	TArray<ANSPlayerState*> Output;

	auto FilteredPlayers = PlayerArray.FilterByPredicate([&](const TObjectPtr<APlayerState>& Entry)
	{
		return FGenericTeamId::GetTeamIdentifier(Entry).GetId() == TeamIndex;
	});

	//todo check if here actual need ANSPlayer state, or just player state are good
	for (const auto& Player : FilteredPlayers)
	{
		if (const auto NSPlayer = Cast<ANSPlayerState>(Player))
			Output.Add(NSPlayer);
	}
	
	/*
	for (auto Player : PlayerArray)
	{
		auto NSPlayer = StaticCast<ANSPlayerState*>(Player);
		if (NSPlayer->GetGenericTeamId().GetId() == TeamIndex)
			Output.Add(NSPlayer);
	}*/

	return Output;
}

FPlayerStatistic ANSGameState::GetTeamStatistic(uint8 TeamId)
{
	FPlayerStatistic Statistic;

	auto Team = GetTeam(TeamId);
	for (const auto& Player : Team)
	{
		Statistic += Player->GetPlayerStatistic();
	}
	
	return Statistic;
}

void ANSGameState::GetNextPlayerInTeam(uint8 TeamIndex, ANSPlayerState*& NextPlayerInTeam, bool bNext, bool bLifePlayer)
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
		NextIndex = FMath::Wrap(NextIndex, 0, Team.Num());

		if (!bLifePlayer || Team[NextIndex]->IsLife())
		{
			NextPlayerInTeam = Team[NextIndex];
			return;
		}
	}
	
	NextPlayerInTeam = nullptr;
	return;
}




//~==============================================================================================
// Match timer

float ANSGameState::GetMatchTimerRemaining()
{
	if (MatchState==EMatchState::InProgress)
	{
		return MatchTimeLimit.GetTotalSeconds() - MatchTime;
	}
	
	return -1.f;
}

