// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/TabMenu.h"


#include "Game/NSPlayerState.h"
#include "Game/NSGameState.h"
#include "GameFramework/GameStateBase.h"

void UTabMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GameState = GetWorld()->GetGameState<ANSGameState>();

	UpdateRows();
}

void UTabMenu::UpdateRows()
{
	auto PlayerArray = GetWorld()->GetGameState()->PlayerArray;

	int32 RowIndex = 0;
	for (const auto& Player : PlayerArray) 
	{
		auto PlayerState = Cast<ANSPlayerState>(Player);
			
		//reinit row, or create new
		if (Rows.IsValidIndex(RowIndex))
		{
			Rows[RowIndex]->Init(PlayerState);
		}
		else
		{
			auto Row = CreateWidget<UTabMenu_Row>(this, TabMenuRowClass);

			Row->Init(PlayerState);
    			
			//add in widgets array
			Rows.Add(Row);
		}
		
		RowIndex++;
	}
	//delete other rows
	Rows.SetNum(PlayerArray.Num(), true);
	
	//send bp event add widgets on table
	BP_RowsUpdated();
}
