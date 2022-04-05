// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSPlayerState.h"

#include "Game/NSGameMode.h"
#include "Net/UnrealNetwork.h"

void ANSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSPlayerState, PlayerStatistic);
	DOREPLIFETIME(ANSPlayerState, TeamID);
}

ANSPlayerState::ANSPlayerState()
{
	SetGenericTeamId(FGenericTeamId(0));
}

void ANSPlayerState::BeginPlay()
{
	Super::BeginPlay();


}

void ANSPlayerState::OnCharacterDeath()
{
	CharacterDeadDelegate.Broadcast(GetPawn());
	
	//notify gamemode about death
	if (GetWorld()->IsServer())
	{
		if (auto GM = Cast<ANSGameMode>(GetWorld()->GetAuthGameMode()))
			GM -> CharacterKilled(GetPawn());
	}
	bDeath = true;
}

bool ANSPlayerState::IsLife()
{
	return !bDeath;
}

void ANSPlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	IGenericTeamAgentInterface::SetGenericTeamId(NewTeamID);

	TeamID = NewTeamID;
}

FGenericTeamId ANSPlayerState::GetGenericTeamId() const
{
	return TeamID;
}

FGenericTeamId ANSPlayerState::GetTeamID()
{
	return GetGenericTeamId();
}

EGameTeam ANSPlayerState::GetTeamID_Verbose()
{
	auto L_TeamID = GetGenericTeamId().GetId();
	return EGameTeam(L_TeamID);
}

void ANSPlayerState::RespawnHandle_Implementation()
{
	CharacterRespawnDelegate.Broadcast();
	
	bDeath = false;
}


//~==============================================================================================
// PlayerStatistic

void ANSPlayerState::AddKill(int32 Count)
{
	PlayerStatistic.KillCount += Count;
}

void ANSPlayerState::AddAssist()
{
	PlayerStatistic.AssistCount++;
}

void ANSPlayerState::AddDeath()
{
	PlayerStatistic.DeathCount++;
}

void ANSPlayerState::OnRep_PlayerStatistic()
{
    // notify client about statistic is update
	PlayerStatisticUpdateDelegate.Broadcast();
}
