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
	SetActorTickEnabled(true);
	SetActorTickInterval(1.f);

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
}

void ANSGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
	if (MatchState == EMatchState::WaitingToStart)
	{
		MatchTime--;
		
		if (MatchTime<=0 && HasAuthority())
		{
			Cast<ANSGameMode>(GetWorld()->GetAuthGameMode())->StartMatch();
		}
	} else
	if (MatchState == EMatchState::InProgress)
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

void ANSGameState::OnCharacterDeath(FDamageInfo DamageInfo)
{

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

TArray<ANSPlayerState*> ANSGameState::GetTeam(uint8 TeamIndex)
{
	TArray<ANSPlayerState*> Output;
	
	for (auto Player : PlayerArray)
	{
		auto NSPlayer = StaticCast<ANSPlayerState*>(Player);
		if (NSPlayer->GetGenericTeamId().GetId() == TeamIndex)
			Output.Add(NSPlayer);
	}

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

	//if Empty return failed result
	if (Team.Num() <= 0)
	{
		NextPlayerInTeam = nullptr;
		return;
	}

	//if Previous actor not set
	if (!NextPlayerInTeam)
	{
		//get first actor in team, or first life actor if need
		for (const auto& Player : Team)
		{
			if (!bLifePlayer || Player->IsLife())
			{
				NextPlayerInTeam = Player;
				return;
			}
		}
		NextPlayerInTeam = nullptr;
		return;
	}
	
	
	auto PreviousPlayerInTeam = NextPlayerInTeam;
	//Get his index in array
	int32 PreviousActorIndex = -1;
	if (PreviousPlayerInTeam)
	{
		for (int32 i = 0; i < Team.Num(); i++)
		{	
			if (Team[i] == PreviousPlayerInTeam)
			{
				PreviousActorIndex = i;
				break;
			}
		}
	}

	//if previous actor not found
	if (PreviousActorIndex == -1)
	{
		//get first actor in team, or first life actor if need
		for (const auto& Player : Team)
		{
			if (!bLifePlayer || Player->IsLife())
			{
				NextPlayerInTeam = Player;
				return;
			}
		}
		NextPlayerInTeam = nullptr;
		return;
	}

	
	//if index successfully founded founded
	
	//return next or previous element, life if need
	if (bNext)
	{
		int32 NextIndex = (PreviousActorIndex+1 < Team.Num()) ? PreviousActorIndex+1 : 0;
		while (NextIndex != PreviousActorIndex)
		{
			if (!bLifePlayer || Team[NextIndex]->IsLife())
			{
				NextPlayerInTeam = Team[NextIndex];
				return;
			}
			NextIndex = (NextIndex+1 < Team.Num()) ? NextIndex+1 : 0;
		}

		NextPlayerInTeam = nullptr;
		return;
	}
	else
	{
		int32 PreviousIndex = (PreviousActorIndex-1 >= 0) ? PreviousActorIndex-1 : Team.Num()-1;
		
		while (PreviousIndex != PreviousActorIndex)
		{
			if (!bLifePlayer || Team[PreviousIndex]->IsLife())
			{
				NextPlayerInTeam = Team[PreviousIndex];
				return;
			}
			PreviousIndex = (PreviousIndex-1 >= 0) ? PreviousIndex-1 : Team.Num()-1;
		}

		NextPlayerInTeam = nullptr;
		return;
	}
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

