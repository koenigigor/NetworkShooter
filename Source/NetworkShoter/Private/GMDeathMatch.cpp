// Fill out your copyright notice in the Description page of Project Settings.


#include "GMDeathMatch.h"

void AGMDeathMatch::CharacterKilled(APawn* WhoKilled, AController* InstigatedBy, AActor* DamageCauser)
{
	Super::CharacterKilled(WhoKilled, InstigatedBy, DamageCauser);

	//respawn after delay when he killed
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGMDeathMatch::RespawnDeathPlayer, RespawnDelay);
}

void AGMDeathMatch::RespawnDeathPlayer()
{
	if (DeathControllers.IsValidIndex(0))
	{
		SpawnPlayer(DeathControllers[0]);
		DeathControllers.RemoveAt(0);
	}
}
