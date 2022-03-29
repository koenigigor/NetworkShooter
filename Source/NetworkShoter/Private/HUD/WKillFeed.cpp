// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WKillFeed.h"

#include "Game/NSGameState.h"
#include "Game/NSPlayerState.h"
#include "HUD/WKillFeed_Row.h"

void UWKillFeed::NativeConstruct()
{
	Super::NativeConstruct();

	if (!bConstructed)
	if (auto NSGameState = GetWorld()->GetGameState<ANSGameState>())
	{
		for (const auto& Player : NSGameState->PlayerArray)
		{
			if (auto NSPlayer = Cast<ANSPlayerState>(Player))
			{
				NSPlayer->CharacterDeadDelegate.AddDynamic(this, &UWKillFeed::OnSomebodyKilled);
			}
		}
		//todo new player bind/unbind
	}
	bConstructed=true;
}

void UWKillFeed::OnSomebodyKilled(APawn* WhoKilled)
{
	//get kill info
	auto NSGameState = GetWorld()->GetGameState<ANSGameState>();
	auto KillInfo = NSGameState->GetKillInfo(WhoKilled);

	//spawn new row
	auto Row = CreateWidget<UWKillFeed_Row>(this, RowClass);
	Row->Init(KillInfo);
	
	//send new row into blueprint
	BP_NewRowCreated(Row);
}
