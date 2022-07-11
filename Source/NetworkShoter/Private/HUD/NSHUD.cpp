// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/NSHUD.h"

#include "Blueprint/UserWidget.h"
#include "Game/NSGameState.h"
#include "Game/PCNetShooter.h"
#include "HUD/TabMenu.h"
#include "HUD/HUDWidget.h"
#include "Pawn/NSSpectator.h"
#include "Pawn/ShooterPlayer.h"

void ANSHUD::BeginPlay()
{
	Super::BeginPlay();
	
	//spawn widgets
	if (IsValid(PreMatchWidgetClass))
		PreMatchWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PreMatchWidgetClass);

	if (IsValid(CharacterHUDClass))
		CharacterHUD = CreateWidget<UHUDWidget>(GetOwningPlayerController(), CharacterHUDClass);

	if (IsValid(SpectatorHUDClass))
		SpectatorHUD = CreateWidget<UHUDWidget>(GetOwningPlayerController(), SpectatorHUDClass);
	
	if (IsValid(TabMenuClass))
		TabMenu = CreateWidget<UTabMenu>(GetOwningPlayerController(), TabMenuClass);
	
	if (IsValid(PostMatchWidgetClass))
		PostMatchWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PostMatchWidgetClass);

	if (IsValid(PauseMenuClass))
		PauseMenu = CreateWidget<UUserWidget>(GetOwningPlayerController(), PauseMenuClass);

	if (IsValid(SharedHUDClass))
	{
		SharedHUD = CreateWidget<UUserWidget>(GetOwningPlayerController(), SharedHUDClass);
		SharedHUD->AddToViewport();
	}
	
	
	//bind to match start/end events
	if (auto NSGameState = GetWorld()->GetGameState<ANSGameState>())
	{
		NSGameState->MatchStartDelegate.AddDynamic(this, &ANSHUD::OnMatchStart);
		NSGameState->MatchEndDelegate.AddDynamic(this, &ANSHUD::OnMatchEnd);

		if (!NSGameState->HasMatchStarted() && PreMatchWidget)
		{
			PreMatchWidget->AddToViewport();
		}
	}

	if (const auto PC = Cast<APCNetShooter>(PlayerOwner.Get()))
	{
		PC->OnLocalPause.AddDynamic(this, &ThisClass::TogglePauseMenu);
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

void ANSHUD::OnPossess(APawn* InPawn)
{
	UUserWidget* HUDtoActivate = nullptr;
	
	//if possess in character show main hud
	if (Cast<AShooterPlayer>(InPawn))
	{
		HUDtoActivate = CharacterHUD;
	}
	
	//if possess in spectator show spectator hud
	if (Cast<ANSSpectator>(InPawn))
	{
		HUDtoActivate = SpectatorHUD;
	}

	
	//if need activate another HUD replace it
	if (HUDtoActivate && HUDtoActivate != CurrentActiveHUD)
   	{
		if (CurrentActiveHUD)
    		CurrentActiveHUD -> RemoveFromParent();
		
    	CurrentActiveHUD = HUDtoActivate;
		CurrentActiveHUD -> AddToViewport();
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

void ANSHUD::TogglePauseMenu(bool OnPause)
{
	if (!PauseMenu) return;
	
	if (OnPause)
	{
		PauseMenu->AddToViewport(10);
	}
	else
	{
		PauseMenu->RemoveFromViewport();
	}
}
