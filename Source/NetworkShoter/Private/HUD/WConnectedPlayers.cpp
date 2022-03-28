// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WConnectedPlayers.h"

#include "Game/NSGameState.h"
#include "HUD/WConnectedPlayer.h"

void UWConnectedPlayers::NativeConstruct()
{
	Super::NativeConstruct();

	//do once
	if (bConstructed) return;
	bConstructed = true;
	
	if (auto NSGameState = GetWorld() -> GetGameState<ANSGameState>())
	{
		NSGameState->PlayerAddedDelegate.AddDynamic(this, &UWConnectedPlayers::PlayerChanged);
		NSGameState->PlayerRemovedDelegate.AddDynamic(this, &UWConnectedPlayers::PlayerChanged);
	}
}

void UWConnectedPlayers::PlayerChanged(APlayerState* Player)
{
	//GetPlayer array
	auto PlayerArray = GetWorld()->GetGameState()->PlayerArray;

	int32 MaxCount = FMath::Max(PlayerWidgetsArray.Num(), PlayerArray.Num());

	for (int32 i = 0; i!=MaxCount; i++)
	{
		if (PlayerArray.IsValidIndex(i) && PlayerWidgetsArray.IsValidIndex(i))
		{
			PlayerWidgetsArray[i] -> Refresh(PlayerArray[i]);
		}
		if (PlayerArray.IsValidIndex(i) && !PlayerWidgetsArray.IsValidIndex(i))
		{
			auto NewPlayerWidget = CreateWidget<UWConnectedPlayer>(this, WConnectedPlayerClass);
			NewPlayerWidget->Refresh(PlayerArray[i]);
			PlayerWidgetsArray.Add(NewPlayerWidget);
		}
		if (!PlayerArray.IsValidIndex(i) && PlayerWidgetsArray.IsValidIndex(i))
		{
			//clear other elements
			PlayerWidgetsArray.SetNum(i,true);
			break;
		}
	}
	BP_Refresh();
}
