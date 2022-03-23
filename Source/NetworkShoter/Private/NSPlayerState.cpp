// Fill out your copyright notice in the Description page of Project Settings.


#include "NSPlayerState.h"

#include "NSGameMode.h"
#include "PCNetShooter.h"
#include "Net/UnrealNetwork.h"

void ANSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSPlayerState, PlayerStatistic);
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

void ANSPlayerState::RespawnHandle_Implementation()
{
	CharacterRespawnDelegate.Broadcast();
	
	bDeath = false;
}


/** Statistic **/

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
