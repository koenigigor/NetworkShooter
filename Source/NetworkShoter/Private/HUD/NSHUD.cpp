// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/NSHUD.h"

#include "Blueprint/UserWidget.h"
#include "Game/NSGameState.h"
#include "HUD/TabMenu.h"

void ANSHUD::BeginPlay()
{
	Super::BeginPlay();
	
	//spawn widgets
	if (IsValid(PreMatchWidgetClass))
		PreMatchWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PreMatchWidgetClass);
	
	if (IsValid(TabMenuClass))
		TabMenu = CreateWidget<UTabMenu>(GetOwningPlayerController(), TabMenuClass);
	
	if (IsValid(PostMatchWidgetClass))
		PostMatchWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PostMatchWidgetClass);

	
	//bind to match start/end events
	if (auto NSGameState = GetWorld()->GetGameState<ANSGameState>())
	{
		NSGameState->MatchStartDelegate.AddDynamic(this, &ANSHUD::OnMatchStart);
		NSGameState->MatchEndDelegate.AddDynamic(this, &ANSHUD::OnMatchEnd);

		if (!NSGameState->HasMatchStarted() && PreMatchWidget)
		{
			PreMatchWidget->AddToViewport();
		}
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("HUD: NSGameState not found"))
	}
}

void ANSHUD::OnMatchStart()
{
	if (PreMatchWidget)
	{
		PreMatchWidget->RemoveFromParent();
	}
}

void ANSHUD::OnMatchEnd()
{
	if (PostMatchWidget)
	{
		PostMatchWidget -> AddToViewport();
	}
}

void ANSHUD::ShowTabMenu(const bool bShow)
{
	if (!TabMenu) { return; }

	if (bShow)
	{
		TabMenu->AddToViewport();
	}
	else
	{
		TabMenu->RemoveFromParent();
	}
}
