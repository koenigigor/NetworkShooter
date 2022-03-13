// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NSGameState.h"

#include "Net/UnrealNetwork.h"

void ANSGameState::AddPlayerPawn(APawn* Pawn)
{
	//get team name

	//add pawn in team array
}

void ANSGameState::RemovePawn(APawn* Pawn)
{
	//get team name

	//remove pawn from team array

	//client rep notify on removed (for swap spectator)
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
