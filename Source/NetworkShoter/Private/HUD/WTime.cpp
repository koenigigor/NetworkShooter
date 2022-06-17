// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WTime.h"

#include "Game/NSGameState.h"


void UWTime::NativeConstruct()
{
	Super::NativeConstruct();

	GameState = GetWorld()->GetGameState<ANSGameState>();
	ensure(GameState);
}

float UWTime::GetMatchTime() const
{
	return GameState->GetMatchTime();
}

void UWTime::GetReadableMatchTime(int32& Minutes, int32& Seconds)
{
	const float MatchTime = GetMatchTime();
	Seconds = FMath::RoundToInt(MatchTime) % 60;
	Minutes = (FMath::RoundToInt(MatchTime) - Seconds) / 60;
}

float UWTime::GetMatchTimeProgress() const
{
	return GameState->GetMatchTimeAbsolute();
}
