// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/TabMenu.h"

#include <string>

#include "NSPlayerState.h"
#include "Game/NSGameState.h"
#include "GameFramework/GameStateBase.h"

void UTabMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GameState = GetWorld()->GetGameState<ANSGameState>();

	auto PlayerArray = GetWorld()->GetGameState()->PlayerArray;
    	for (const auto& Player : PlayerArray)
    	{
    		if (auto PlayerState = Cast<ANSPlayerState>(Player))
    		{
    			//create and init 
    			auto Row = CreateWidget<UTabMenu_Row>(this, TabMenuRowClass);

				Row->Init(PlayerState);
    			
    			//add in widgets array
    			Rows.Add(Row);
    		}
    	}
		//send bp event add widgets on table
		BP_RowsUpdated();
}

void UTabMenu::OnPawnSpawned(ANSPlayerState* PlayerState)
{
	//enable line
	
}

void UTabMenu::OnPawnDeath(ANSPlayerState* PlayerState)
{
	//get player state line
	//for (const auto& Widget : WidgetList)
	{
		//Widget
	}
	
}


/** MATCH TIME **/

float UTabMenu::GetMatchTime()
{
	if (GameState)
	{
		float MatchTime = GetWorld()->TimeSeconds - GameState->MatchStartTime;
		return MatchTime;
	}
	return -1.f;
}

FString UTabMenu::GetMatchStatusAndTime()
{
	FString Output = "";
	if (!GameState) { return Output; }
	
	auto MatchState = GameState->MatchState;
	if (MatchState == EMatchState::WaitingToStart)
	{
		Output = "Wait start match";
		//time to start
		
		return Output;
	}
	
	if (MatchState == EMatchState::InProgress)
	{
		return FString::SanitizeFloat(GetMatchTime(), 0);
	}

	if (MatchState == EMatchState::PostMatch)
	{
		Output = "Match ended";
		return Output;
	}

	return Output;
}

float UTabMenu::GetMatchTimeProgress()
{
	auto MatchState = GameState->MatchState;
	if (MatchState == EMatchState::WaitingToStart)
	{
		return 0.f;
	}
	
	if (MatchState == EMatchState::InProgress)
	{
		return GetMatchTime() / GameState->MatchTimeLimit.GetSeconds();
	}

	if (MatchState == EMatchState::PostMatch)
	{
		return 1.f;
	}
	
	return 0.f;
}
