// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WTime.h"

#include "Game/NSGameState.h"


void UWTime::NativeConstruct()
{
	Super::NativeConstruct();

	GameState = GetWorld()->GetGameState<ANSGameState>();
}

float UWTime::GetMatchTime()
{
	if (GameState)
	{
		float MatchTime = GetWorld()->TimeSeconds - GameState->MatchStartTime;
		return MatchTime;
	}
	return -1.f;
}

void UWTime::GetReadableMatchTime(int32& Minutes, int32& Seconds)
{
	float MatchTime = GetMatchTime();
	Seconds = FMath::RoundToInt(MatchTime) % 60;
	Minutes = (FMath::RoundToInt(MatchTime) - Seconds) / 60;
}
