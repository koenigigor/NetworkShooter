// Fill out your copyright notice in the Description page of Project Settings.


#include "WConnectedPlayers.h"

#include "Components/VerticalBox.h"
#include "Game/NSGameState.h"

void UWConnectedPlayers::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (const auto NSGameState = GetWorld() -> GetGameState<ANSGameState>())
    {
		UE_LOG(LogTemp, Display, TEXT("UWConnectedPlayers::NativeOnInitialized"))
    	NSGameState->PlayerAddedDelegate.AddDynamic(this, &UWConnectedPlayers::OnPlayersCountChange);
    	NSGameState->PlayerRemovedDelegate.AddDynamic(this, &UWConnectedPlayers::OnPlayersCountChange);
		OnPlayersCountChange(nullptr);
    }
}

void UWConnectedPlayers::OnPlayersCountChange(APlayerState* Player)
{
	//Reuse spawned widget
	const auto& PlayerArray = GetWorld()->GetGameState()->PlayerArray;
	const int32 MaxCount = FMath::Max(PlayerWidgetsArray.Num(), PlayerArray.Num());

	PlayersBox->ClearChildren();
	for (int32 i = 0; i!=MaxCount; i++)
	{
		if (PlayerArray.IsValidIndex(i) && PlayerWidgetsArray.IsValidIndex(i))
		{
			//UE_LOG(LogTemp, Display, TEXT("%s, Reinit Widget") )
			PlayerWidgetsArray[i] -> Refresh(PlayerArray[i]);
			PlayersBox->AddChildToVerticalBox(PlayerWidgetsArray[i]);
		}
		if (PlayerArray.IsValidIndex(i) && !PlayerWidgetsArray.IsValidIndex(i))
		{
			auto NewPlayerWidget = CreateWidget<UWConnectedPlayer>(this, WConnectedPlayerClass);
			NewPlayerWidget->Refresh(PlayerArray[i]);
			PlayerWidgetsArray.Add(NewPlayerWidget);
			PlayersBox->AddChildToVerticalBox(NewPlayerWidget);
		}
		if (!PlayerArray.IsValidIndex(i) && PlayerWidgetsArray.IsValidIndex(i))
		{
			//clear other elements
			PlayerWidgetsArray.SetNum(i,true);
			break;
		}
	}
}
