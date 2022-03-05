// Fill out your copyright notice in the Description page of Project Settings.


#include "NSGameMode.h"

void ANSGameMode::CharacterKilled(APawn* KillInstigator, AActor* WhoKilled, AActor* Causer)
{
	//do something

	//Broadcast
}

TArray<ANSPlayerStart*> ANSGameMode::GetFreePlayerStarts(FName CommandName)
{
	TArray<ANSPlayerStart*> FreePoints;

	//Get All Actors of class
	
	return FreePoints;
}

APawn* ANSGameMode::SpawnPlayer(APlayerController* Controller)
{
	//todo
	return nullptr;
}

APawn* ANSGameMode::SpawnSpectator(APlayerController* Controller)
{
	//spawn spectator pawn
	//transform?

	//possess
	
	return nullptr;
}
