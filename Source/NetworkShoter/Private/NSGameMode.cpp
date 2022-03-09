// Fill out your copyright notice in the Description page of Project Settings.


#include "NSGameMode.h"

void ANSGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerDeath.AddDynamic(this, &ANSGameMode::CharacterKilled);
}

void ANSGameMode::CharacterKilled(APawn* WhoKilled, AController* InstigatedBy, AActor* DamageCauser)
{
	//do something

	UE_LOG(LogTemp, Warning, TEXT("Gamemoge say: %s is died"), *WhoKilled->GetName())
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
