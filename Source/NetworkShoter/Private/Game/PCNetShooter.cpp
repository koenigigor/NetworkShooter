// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PCNetShooter.h"

#include "NSAbilitySystemComponent.h"
#include "Game/Components/ChatController.h"
#include "Game/Components/MapVoteController.h"
#include "Game/NSPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/SpectatorPawn.h"
#include "HUD/NSHUD.h"
#include "Pawn/ShooterPlayer.h"


void APCNetShooter::NotifyReceiveDamage_Implementation(float Damage, FVector FromDirection, FName InstigatorName,
	AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("PC: client receive damage %f, from %s"), Damage, *InstigatorName.ToString())
}

APawn* APCNetShooter::SpawnSpectator()
{
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

ANSHUD* APCNetShooter::GetNSHUD()
{
	if (!NSHUD) 
	{
		//protect for cast every call
		NSHUD = Cast<ANSHUD>(GetHUD());
	}
	
	return NSHUD;
}

void APCNetShooter::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//if it local client possess (spectator)
	if (GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		OnRep_Pawn();
	}
}

void APCNetShooter::OnRep_Pawn()
{
	Super::OnRep_Pawn();
//пока не стоит объявлять делегат ради 2х функций
	
	BP_ClientOnPossess(GetPawn());
    
	GetNSHUD()->OnPossess(GetPawn());

	
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
	

	//init ASC on client for use local predicted abilities	
	AShooterPlayer* ShooterPlayer = Cast<AShooterPlayer>(P);
	if (ShooterPlayer)
	{
		ShooterPlayer->GetAbilitySystemComponent()->InitAbilityActorInfo(ShooterPlayer, ShooterPlayer);
	}
}


void APCNetShooter::SendChatMessage_Implementation(const FString& Message)
{
	if (auto ChatController = GetWorld()->GetGameState()->FindComponentByClass<UChatController>())
	{
		ChatController->SendMessage_Player(Message, GetPlayerState<ANSPlayerState>());
	}
}

void APCNetShooter::VoteForMap_Implementation(FName MapRow, bool Up)
{
	if (auto MapVoteController = GetWorld()->GetGameState()->FindComponentByClass<UMapVoteController>())
	{
		MapVoteController->VoteForMap(this, MapRow, Up);
	}
}
