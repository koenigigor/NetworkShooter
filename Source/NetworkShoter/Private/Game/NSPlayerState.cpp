// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSPlayerState.h"

#include "Game/NSGameMode.h"
#include "Net/UnrealNetwork.h"

void ANSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSPlayerState, PlayerStatistic);
	DOREPLIFETIME(ANSPlayerState, TeamIndex);
}

void ANSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	CharacterDeadDelegate.AddDynamic(this, &ANSPlayerState::OnCharacterDeath);
}

void ANSPlayerState::OnCharacterDeath()
{
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
	return bDeath;
}

void ANSPlayerState::RespawnHandle_Implementation()
{
	CharacterRespawnDelegate.Broadcast();
	
	bDeath = false;
}


//~==============================================================================================
// PlayerStatistic

void ANSPlayerState::AddKill()
{
	PlayerStatistic.KillCount++;
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
