// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/NSSpectator.h"

#include "Game/NSGameState.h"
#include "Game/NSPlayerState.h"


ANSSpectator::ANSSpectator()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
}

void ANSSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ANSSpectator::GetNextPlayerToAttach(bool bNext)
{
	auto NSGameState = GetWorld() -> GetGameState<ANSGameState>();
	
	auto NSPlayerState = GetInstigatorController()->GetPlayerState<ANSPlayerState>();
	
	if (!(NSGameState && NSPlayerState)){ return; }
	
	int32 TeamIndex = NSPlayerState->TeamIndex;
	NSGameState -> GetNextPlayerInTeam(TeamIndex, CurrentAttachedPlayer);
}

//~==============================================================================================
// Control Spectating

void ANSSpectator::ChangeAttachedActor(bool bNext)
{
	if (SpectatorMode == ESpectatorMode::Free) { return; }
	
	GetNextPlayerToAttach(bNext);
	
	SetSpectatorMode(SpectatorMode);
}

void ANSSpectator::SetSpectatorMode(ESpectatorMode Mode)
{
	//exit from current mode
	if (SpectatorMode == ESpectatorMode::AttachToActor)
	{
		ExitModeAttachToActor();
	}
	if (SpectatorMode == ESpectatorMode::AroundActor)
	{
		ExitModeAroundActor();
	}
	
	SpectatorMode = Mode;

	//enter to mew mode
	if (Mode == ESpectatorMode::AttachToActor)
	{
		SetModeAttachToActor();
	}
	if (Mode == ESpectatorMode::AroundActor)
	{
		SetModeAroundActor();
	}
}


//~==============================================================================================
// spectator modes Begin/End

void ANSSpectator::SetModeAttachToActor()
{
	//get next actor to attach
	if (!CurrentAttachedPlayer)
		GetNextPlayerToAttach();

	if (!CurrentAttachedPlayer)
		return;
	
	AActor* ActorToAttach = CurrentAttachedPlayer->GetPawn();
	
	if (!ActorToAttach)
		return;
	
	//Set actor camera view
	if (GetController())
	{
		if (auto PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerController -> SetViewTarget(ActorToAttach);
		}
	}
}

void ANSSpectator::SetModeAroundActor()
{
	//get next actor to attach
	if (!CurrentAttachedPlayer)
		GetNextPlayerToAttach();
	
	if (!CurrentAttachedPlayer)
		return;

	AActor* ActorToAttach = CurrentAttachedPlayer->GetPawn();
	
	if (!ActorToAttach)
		return;

	//Attach SpectatorPawn to Actor
	this->AttachToActor(ActorToAttach, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	//set movement
}

void ANSSpectator::ExitModeAttachToActor()
{
	//return camera to self
	if (GetController())
	{
		if (auto PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerController -> SetViewTarget(this);
		}
	}
}

void ANSSpectator::ExitModeAroundActor()
{
	
}
