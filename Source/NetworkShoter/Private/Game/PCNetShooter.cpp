// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PCNetShooter.h"

#include "GAS/NSAbilitySystemComponent.h"
#include "Game/Components/ChatController.h"
#include "Game/Components/MapVoteController.h"
#include "Game/NSPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/SpectatorPawn.h"
#include "HUD/NSHUD.h"
#include "Pawn/ShooterPlayer.h"



APawn* APCNetShooter::SpawnSpectator()
{
	if (HasAuthority()) return nullptr;
	
	//spawn spectator pawn
	APawn* SpawnedSpectator = nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnParams.ObjectFlags |= RF_Transient;	// We never want to save spectator pawns into a map
	auto CastController = StaticCast<AActor*>(this);

	auto SpectatorClass = GetWorld()->GetGameState()->SpectatorClass;
	SpawnedSpectator = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, CastController->GetActorLocation(), GetControlRotation(), SpawnParams);

	bCanPossessWithoutAuthority = true;
	//possess
	if (GetPawn())
	{
		UnPossess();
	}
	Possess(SpawnedSpectator);

	bCanPossessWithoutAuthority = false;
	
	return SpawnedSpectator;
}

ANSHUD* APCNetShooter::GetNSHUD() const
{
	return GetHUD<ANSHUD>();
}


void APCNetShooter::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
	
	//it not calls on dedicated

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

	//is like on rep pawn?
	BP_ClientOnPossess(GetPawn());
    
	GetNSHUD()->OnPossess(GetPawn());
	

	//init ASC on client for use local predicted abilities	
	if (const auto ShooterPlayer = Cast<AShooterPlayer>(P))
	{
		ShooterPlayer->GetAbilitySystemComponent()->InitAbilityActorInfo(ShooterPlayer, ShooterPlayer);
	}
}

void APCNetShooter::ShowMouse(bool bShow)
{
	//we can want show mouse from different sources
	const auto bPrevState = bMouseIsShoved;
	bMouseIsShoved = FMath::Max<int8>(bMouseIsShoved + (bShow ? +1 : -1), 0);

	if (bMouseIsShoved && bPrevState) return;

	bShowMouseCursor = bMouseIsShoved;
	if (bShowMouseCursor)
	{
		int32 ViewportX, ViewportY;
		GetViewportSize(ViewportX, ViewportY);
		SetMouseLocation(ViewportX/2, ViewportY/2);

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
	}
}

void APCNetShooter::ResetShowMouse()
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void APCNetShooter::ToggleLocalPause()
{
	bOnPause = !bOnPause;
	ShowMouse(bOnPause);
	SetPause(bOnPause);
	if (GetPawnOrSpectator())
	{
		bOnPause ? GetPawnOrSpectator()->DisableInput(this) : GetPawnOrSpectator()->EnableInput(this);
	}
	OnLocalPause.Broadcast(bOnPause);
}


void APCNetShooter::SendChatMessage_Implementation(const FString& Message)
{
	if (const auto ChatController = GetWorld()->GetGameState()->FindComponentByClass<UChatController>())
	{
		ChatController->SendMessage_Player(Message, GetPlayerState<ANSPlayerState>());
	}
}

void APCNetShooter::VoteForMap_Implementation(FName MapRow, bool Up)
{
	if (const auto MapVoteController = GetWorld()->GetGameState()->FindComponentByClass<UMapVoteController>())
	{
		MapVoteController->VoteForMap(this, MapRow, Up);
	}
}
