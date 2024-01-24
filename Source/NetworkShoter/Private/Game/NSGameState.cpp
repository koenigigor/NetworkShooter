// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "Game/NSGameMode.h"
#include "Game/NSPlayerState.h"
#include "Game/Components/DamageHistoryComponent.h"
#include "Game/Components/TeamSetupManager.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


ANSGameState::ANSGameState()
{
	SetActorTickEnabled(false);

	DamageHistory = CreateDefaultSubobject<UDamageHistoryComponent>(TEXT("DamageHistory"));
	TeamManager = CreateDefaultSubobject<UTeamSetupManager>(TEXT("TeamManager"));
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

	if (GetWorld()->GetNetMode() == NM_DedicatedServer)
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_DAMAGE, this, &ThisClass::MulticastDamageInfo);
		MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_DEATH, this, &ThisClass::MulticastDamageInfo);
		MessageSubsystem.RegisterListener(TAG_MESSAGE_SYSTEM_HEAL, this, &ThisClass::MulticastDamageInfo);
	}
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

FPlayerStatistic ANSGameState::GetTeamStatistic(uint8 TeamId) const
{
	FPlayerStatistic Statistic;

	auto Team = TeamManager->GetTeam(TeamId);
	for (const auto& Player : Team)
	{
		Statistic += Player->GetPlayerStatistic();
	}
	
	return Statistic;
}

//~==============================================================================================
// Match timer

float ANSGameState::GetMatchTimeAbsolute()
{
	switch (GetMatchState())
	{
	case EMatchState::WaitingToStart:
		{
			return 1 - GetMatchTime()/WaitStartMatchTime;
			break;
		}
	case EMatchState::InProgress:
		{
			return GetMatchTime() / MatchTimeLimit.GetTotalSeconds();
			break;
		}
	case EMatchState::PostMatch:
		{
			return 1;
			break;
		}
	default:
		{
			return 0;
		}	
	}
}

float ANSGameState::GetMatchTimerRemaining() const
{
	if (MatchState==EMatchState::InProgress)
	{
		return MatchTimeLimit.GetTotalSeconds() - GetMatchTime();
	}
	
	return -1.f;
}

void ANSGameState::MulticastDamageInfo_Implementation(FGameplayTag Tag, const FDamageInfo& DamageInfo)
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
		MessageSystem.BroadcastMessage(Tag, DamageInfo);
	}
}

