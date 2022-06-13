// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSPlayerState.h"

#include "GAS/NSAbilitySystemComponent.h"
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
	AbilitySystemComponent = CreateDefaultSubobject<UNSAbilitySystemComponent>("AbilitySystemComponent");
}

void ANSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		for (const auto& Ability : StartupAbility)
		{
			AbilitySystemComponent->GiveAbility(Ability);
		}
	}
}

void ANSPlayerState::OnCharacterDeath()
{
	CharacterDeadDelegate.Broadcast(GetPawn());
	
	//notify gamemode about death
	if (HasAuthority())
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

UAbilitySystemComponent* ANSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

EGameTeam ANSPlayerState::GetTeamID() const
{	
	return StaticCast<EGameTeam>(GetGenericTeamId().GetId());
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
	PlayerStatisticUpdateDelegate.Broadcast();
}
