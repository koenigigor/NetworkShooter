// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "Net/UnrealNetwork.h"

void ANSGameState::AddPlayerPawn(APawn* Pawn, FName Team)
{
}

void ANSGameState::RemovePawn(APawn* Pawn, FName Team)
{
}

void ANSGameState::OnRep_Team1()
{
}

void ANSGameState::OnRep_Team2()
{
}

void ANSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSGameState, Team1);
	DOREPLIFETIME(ANSGameState, Team2);
}
