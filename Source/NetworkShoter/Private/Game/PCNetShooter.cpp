// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PCNetShooter.h"

#include "EnhancedInputComponent.h"
#include "GAS/NSAbilitySystemComponent.h"
#include "Chat/ChatController.h"
#include "Game/Components/MapVoteController.h"
#include "Game/NSPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "HUD/NSHUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogNSPlayerController, All, All);


APCNetShooter::APCNetShooter()
{
	ChatProxy = CreateDefaultSubobject<UChatProxy>("ChatProxy");
}

void APCNetShooter::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (const auto EI = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (VisibleMouseClickAction)
		{
			EI->BindAction(VisibleMouseClickAction, ETriggerEvent::Started, this, &ThisClass::OnVisibleMouseClick);	//todo bind lambda will be in 5.4
		} else UE_LOG(LogNSPlayerController, Error, TEXT("VisibleMouseClickAction not set"))
	} 
}

void APCNetShooter::OnVisibleMouseClick()
{
	OnVisibleMouseClickNotify.Broadcast();
}


void APCNetShooter::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	//process input hold abilities, peek from Lyra
	if (GetPawn())
	{
		if (auto ASC = GetPawn()->FindComponentByClass<UNSAbilitySystemComponent>())
		{
			ASC->ProcessInputHoldAbilities();
		}
	}
}

void APCNetShooter::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	//Client side possess event, UPD: remember found another bp side client event, try found it again
	BP_ClientOnPossess(GetPawn());

	//init ASC on client for use local predicted abilities
	if (const auto ASC = Cast<IAbilitySystemInterface>(P))
	{
		ASC->GetAbilitySystemComponent()->InitAbilityActorInfo(P, P);
	}
}

void APCNetShooter::UpdateMouseState()
{
	bool bShouldMouse = bOnPause;

	if (!bShouldMouse)
		if (const auto Hud = GetHUD<ANSHUD>())
		{
			bShouldMouse = Hud->HasWindowInFocus() || Hud->HasChatFocus();
		}

	if (!bShouldMouse)
	{
		//ensure(Cast<UEnhancedPlayerInput>(PlayerInput));
		//bShouldMouse = Cast<UEnhancedPlayerInput>(PlayerInput)->GetActionValue(IA).IsNonZero();
		bShouldMouse = bShowMousePressed;
	}
	

	{
		UE_CLOG(bShouldMouse, LogNSPlayerController, Display, TEXT("Show mouse"))
		UE_CLOG(!bShouldMouse, LogNSPlayerController, Display, TEXT("Hide mouse"))

		if (bShowMouseCursor == bShouldMouse) return;
	
		bShowMouseCursor = bShouldMouse;
		if (bShowMouseCursor)
		{
			if (LastMouseLocation == FIntPoint::ZeroValue)
			{
				int32 ViewportX, ViewportY;
				GetViewportSize(ViewportX, ViewportY);
				LastMouseLocation = FIntPoint(ViewportX / 2, ViewportY / 2);
			}
			
			SetMouseLocation(LastMouseLocation.X, LastMouseLocation.Y);
			
			SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		}
		else
		{
			float MouseX, MouseY;
			GetMousePosition(MouseX, MouseY);
			LastMouseLocation = FIntPoint(MouseX, MouseY);
			
			SetInputMode(FInputModeGameOnly());
		}
	}
}

void APCNetShooter::ShowMouseByButton(bool bShow)
{
	UE_CLOG(bShow, LogNSPlayerController, Display, TEXT("Set show mouse by button"))
	UE_CLOG(!bShow, LogNSPlayerController, Display, TEXT("Hide mouse by button"))

	bShowMousePressed = bShow;
	UpdateMouseState();
}

void APCNetShooter::SetInputMode(const FInputModeDataBase& InData)
{
	Super::SetInputMode(InData);

	// Mouse captor valid only in GameOnly Mode, See realisation input mode
	// Dynamic cast throw compile error?
	const bool GameOnly = GetLocalPlayer()->GetSlateOperations().GetMouseCaptor().IsValid();
	if (GameOnly)
	{
		OnSetInputModeGameOnly.Broadcast();
	}
	else
	{
		OnSetInputModeUI.Broadcast();
	}
}

void APCNetShooter::ToggleLocalPause()
{
	bOnPause = !bOnPause;
	SetPause(bOnPause);
	if (GetPawnOrSpectator())
	{
		bOnPause
			? GetPawnOrSpectator()->DisableInput(this)
			: GetPawnOrSpectator()->EnableInput(this);
	}
	OnLocalPause.Broadcast(bOnPause);

	UpdateMouseState();
}


void APCNetShooter::ChangeState(FName NewState)
{
	if (NewState != StateName)
	{
		Super::ChangeState(NewState);

		OnChangeState.Broadcast(NewState);
	}
}

void APCNetShooter::BeginPlayingState()
{
	Super::BeginPlayingState();

	// possess to previous pawn, if cancel spectating mode and was not possess anywhere
	if (HasAuthority())
	{
		constexpr float CheckReturnToPawnDelay = 1.f;
		FTimerHandle ReturnToPawnHandle;
		GetWorldTimerManager().SetTimer(ReturnToPawnHandle, FTimerDelegate::CreateLambda([&]
		{
			if (!GetPawn())
			{
				if (PreviousPawn.IsValid()) Possess(PreviousPawn.Get());
			}
		}), CheckReturnToPawnDelay, false);
	}
}

void APCNetShooter::EndPlayingState()
{
	SpawnLocation = StaticCast<AActor*>(this)->GetActorLocation();		//save current location for spectator was spawn there
	//if (GetPawn()) SpawnLocation = GetPawn()->GetActorLocation();
	PreviousPawn = GetPawn();												//save current pawn if exist, for repossess if need

	//UE_LOG(LogTemp, Error, TEXT("%s, SpawnLocation = %s"), LOG_NET_ROLE_STR, *SpawnLocation.ToString())

	Super::EndPlayingState();
}

void APCNetShooter::VoteForMap_Implementation(FName MapRow, bool Up)
{
	if (const auto MapVoteController = GetWorld()->GetGameState()->FindComponentByClass<UMapVoteController>())
	{
		MapVoteController->VoteForMap(this, MapRow, Up);
	}
}
