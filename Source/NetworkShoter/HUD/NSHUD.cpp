// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/NSHUD.h"

#include "LayerWidget/WindowLayerWidget.h"
#include "Blueprint/UserWidget.h"
#include "Game/NSGameState.h"
#include "Game/PCNetShooter.h"
#include "Widgets/TabMenu.h"
#include "Widgets/HUDWidget.h"
#include "Pawn/ShooterPlayer.h"

DEFINE_LOG_CATEGORY_STATIC(LogNSHUD, All, All);

ANSHUD* ANSHUD::GetNSHUD(APlayerController* PC)
{
	return PC ? PC->GetHUD<ANSHUD>() : nullptr;
}

/** True if any window focused */ 
bool ANSHUD::HasWindowInFocus() const
{
	return WindowLayerWidget->GetWindowCount() > 0;
}

bool ANSHUD::HasChatFocus() const
{
	//todo temp or it's ok???
	return SharedHUD ? SharedHUD->HasFocusedDescendants() : false;
}

void ANSHUD::BeginPlay()
{
	Super::BeginPlay();
	
	//spawn widgets
	{
		if (IsValid(PreMatchWidgetClass))
		{
			PreMatchWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PreMatchWidgetClass);
		} else UE_LOG(LogNSHUD, Error, TEXT("PreMatchWidgetClass not set"))

		if (IsValid(CharacterHUDClass))
		{
			CharacterHUD = CreateWidget<UHUDWidget>(GetOwningPlayerController(), CharacterHUDClass);
		} else UE_LOG(LogNSHUD, Error, TEXT("CharacterHUDClass not set"))

		if (IsValid(SpectatorHUDClass))
		{
			SpectatorHUD = CreateWidget<UHUDWidget>(GetOwningPlayerController(), SpectatorHUDClass);
		} else UE_LOG(LogNSHUD, Error, TEXT("SpectatorHUDClass not set"))
	
		if (IsValid(TabMenuClass))
		{
			TabMenu = CreateWidget<UTabMenu>(GetOwningPlayerController(), TabMenuClass);
		} else UE_LOG(LogNSHUD, Error, TEXT("TabMenuClass not set"))
	
		if (IsValid(PostMatchWidgetClass))
		{
			PostMatchWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PostMatchWidgetClass);
		} else UE_LOG(LogNSHUD, Error, TEXT("PostMatchWidgetClass not set"))

		if (IsValid(PauseMenuClass))
		{
			PauseMenu = CreateWidget<UUserWidget>(GetOwningPlayerController(), PauseMenuClass);
		} else UE_LOG(LogNSHUD, Error, TEXT("PauseMenuClass not set"))

		if (IsValid(SharedHUDClass))
		{
			SharedHUD = CreateWidget<UHUDWidget>(GetOwningPlayerController(), SharedHUDClass);
			SharedHUD->AddToViewport();
		} else UE_LOG(LogNSHUD, Error, TEXT("SharedHUDClass not set"))
	
		if (IsValid(WindowLayerWidgetClass))
		{
			WindowLayerWidget = CreateWidget<UWindowLayerWidget>(GetOwningPlayerController(), WindowLayerWidgetClass);
			WindowLayerWidget->AddToViewport();
		} else UE_LOG(LogNSHUD, Error, TEXT("WindowLayerWidgetClass not set"))
	
		if (IsValid(ContextMenuLayerClass))
		{
			ContextMenuLayer = CreateWidget<UWindowLayerWidget>(GetOwningPlayerController(), ContextMenuLayerClass);
			ContextMenuLayer->AddToViewport(999);
		} else UE_LOG(LogNSHUD, Error, TEXT("ContextMenuCanvasClass not set"))
	}
	
	//bind to match start/end events
	if (const auto NSGameState = GetWorld()->GetGameState<ANSGameState>())
	{
		NSGameState->MatchStartDelegate.AddDynamic(this, &ANSHUD::OnMatchStart);
		NSGameState->MatchEndDelegate.AddDynamic(this, &ANSHUD::OnMatchEnd);

		if (!NSGameState->HasMatchStarted())
		{
			OnPreMatch();
		}
	} else UE_LOG(LogNSHUD, Error, TEXT("Game State not found"))

	if (const auto PC = Cast<APCNetShooter>(PlayerOwner.Get()))
	{
		PC->OnLocalPause.AddDynamic(this, &ThisClass::TogglePauseMenu);
		PC->OnChangeState.AddUObject(this, &ThisClass::OnChangeState);
		PC->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossess);
		PC->OnSetInputModeGameOnly.AddDynamic(this, &ThisClass::OnSetInputModeGameOnly);
		PC->OnVisibleMouseClickNotify.AddDynamic(this, &ThisClass::OnVisibleMouseClick);
	} else UE_LOG(LogNSHUD, Error, TEXT("PC not found"))

	//bind to focus on chat layer, for toggle mouse
	if (SharedHUD)
	{
		//todo rename shared hut to chat only hud?
		SharedHUD->OnAddedToFocusPathDelegate.BindUObject(this, &ThisClass::WantUpdateMouse);
		SharedHUD->OnRemovedFromFocusPathDelegate.BindUObject(this, &ThisClass::WantUpdateMouse);
	} else UE_LOG(LogNSHUD, Error, TEXT("Chat layer not initialized"))

	if (WindowLayerWidget)
	{
		WindowLayerWidget->OnWindowCountChanged.BindUObject(this, &ThisClass::WantUpdateMouse);
	} else UE_LOG(LogNSHUD, Error, TEXT("Window layer not initialized"))
}

void ANSHUD::OnPreMatch()
{
	if (PreMatchWidget)
	{
		PreMatchWidget->AddToViewport();
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

void ANSHUD::TogglePauseMenu(bool OnPause)
{
	if (!PauseMenu) return;
	
	if (OnPause)
	{
		PauseMenu->AddToViewport(10);
	}
	else
	{
		PauseMenu->RemoveFromParent();
	}
}

void ANSHUD::OnChangeState(FName NewState)
{
	UUserWidget* HUDtoActivate = nullptr;
	
	if (NewState == NAME_Spectating)
	{
		HUDtoActivate = SpectatorHUD;
	}
	
	//if need activate another HUD replace it
	if (HUDtoActivate)
	{
		if (CurrentActiveHUD) CurrentActiveHUD -> RemoveFromParent();
		
		CurrentActiveHUD = HUDtoActivate;
		CurrentActiveHUD -> AddToViewport();
	}
}

void ANSHUD::WantUpdateMouse()
{
	UE_LOG(LogTemp, Warning, TEXT("ANSHUD::WantUpdateMouse"))
	if (const auto PC = Cast<APCNetShooter>(PlayerOwner.Get()))
	{
		PC->UpdateMouseState();
	}
}

void ANSHUD::OnPossess(APawn* OldPawn, APawn* NewPawn)
{
	if (!NewPawn) return;

	UUserWidget* HUDtoActivate = nullptr;

	//if possess in character show main hud
	//if (Cast<ACharacter>(NewPawn))
	{
		HUDtoActivate = CharacterHUD;
	}
	
	//if need activate another HUD replace it
	if (HUDtoActivate)
	{
		if (CurrentActiveHUD) CurrentActiveHUD -> RemoveFromParent();
		
		CurrentActiveHUD = HUDtoActivate;
		CurrentActiveHUD -> AddToViewport();
	}	
}

#pragma region Window

void ANSHUD::PushWindow(FGameplayTag WindowTag, UUserWidget* Window)
{
	if (WindowLayerWidget)
	{
		WindowLayerWidget->PushWindow(WindowTag, Window);
	}
}

void ANSHUD::PushChildWindow(UWidget* Parent, UUserWidget* Window, EWindowSnap ParentSnap, bool bHorizontal)
{
	if (WindowLayerWidget)
	{
		WindowLayerWidget->PushWindow(Parent, Window, ParentSnap, bHorizontal);
	}
}

void ANSHUD::RemoveTopWindow()
{
	if (WindowLayerWidget)
	{
		WindowLayerWidget->RemoveTopWindow();
	}
}

void ANSHUD::RemoveAllWindows()
{
	if (WindowLayerWidget)
	{
		WindowLayerWidget->RemoveAllWindows();
	}
}

#pragma endregion Window

#pragma region ContextMenu

void ANSHUD::PushContextMenu(UWidget* InMenu)
{
	ContextMenuLayer->RemoveAllWindows();
	ContextMenuLayer->PushWindow(InMenu);
}

void ANSHUD::PushContextMenuAroundWidget(UWidget* Parent, UWidget* Menu, EWindowSnap ParentSnap, bool bHorizontal)
{
	ContextMenuLayer->RemoveAllWindows();
	ContextMenuLayer->PushWindow(Parent, Menu, ParentSnap, bHorizontal);
}

void ANSHUD::OnVisibleMouseClick()
{
	if (!ContextMenuLayer->IsHovered_Slow())
	{
		ContextMenuLayer->RemoveAllWindows();
	}
}

void ANSHUD::OnSetInputModeGameOnly()
{
	ContextMenuLayer->RemoveAllWindows();
	RemoveAllWindows();
}

#pragma endregion ContextMenu	