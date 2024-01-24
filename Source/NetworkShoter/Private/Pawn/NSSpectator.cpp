// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/NSSpectator.h"

#include "Components/SphereComponent.h"
#include "Game/NSGameState.h"
#include "Game/NSPlayerState.h"
#include "Game/Components/TeamSetupManager.h"
#include "GameFramework/PawnMovementComponent.h"


ANSSpectator::ANSSpectator()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
}

void ANSSpectator::ChangeAttachedActor(bool bNext, bool bOnlyLive)
{
	if (SpectatorMode == ESpectatorMode::Free) { return; }
	
	GetNextPlayerToAttach(bNext, bOnlyLive);
	
	SetSpectatorMode(SpectatorMode);
}

void ANSSpectator::SetSpectatorMode(ESpectatorMode Mode)
{
	switch (SpectatorMode)
	{
	case ESpectatorMode::AttachToActor:
		{
			EndModeAttachToActor();
			break;
		}
	case ESpectatorMode::AroundActor:
		{
			EndModeAroundActor();
			break;
		}
	case ESpectatorMode::Free:
		{
			EndModeFree();
			break;
		}
	default:
		{

		}	
	}
	
	SpectatorMode = Mode;

	switch (Mode)
	{
	case ESpectatorMode::AttachToActor:
		{
			SetModeAttachToActor();
			break;
		}
	case ESpectatorMode::AroundActor:
		{
			SetModeAroundActor();
			break;
		}
	case ESpectatorMode::Free:
		{
			SetModeFree();
			break;
		}
	default:
		{

		}	
	}
}


void ANSSpectator::SetModeFree()
{

}

void ANSSpectator::EndModeFree()
{
	
}

void ANSSpectator::SetModeAttachToActor()
{	
	AActor* AttachTo = GetPawnToAttach();
	if (!AttachTo) return;
	
	//Set actor camera view
	if (const auto PC = GetController<APlayerController>())
	{
		PC -> SetViewTarget(AttachTo);
		
		GetMovementComponent()->SetActive(false);
	}
}

void ANSSpectator::EndModeAttachToActor()
{
	GetMovementComponent()->SetActive(true);
	
	//return camera to self
	if (const auto PC = GetController<APlayerController>())
	{
		PC -> SetViewTarget(this);
	}
}

void ANSSpectator::SetModeAroundActor()
{
	const auto AttachTo = GetPawnToAttach();
	if (!AttachTo) return;

	GetCollisionComponent()->SetSimulatePhysics(false);
	GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SetActorLocation(AttachTo->GetPawnViewLocation());
	AttachToActor(AttachTo, FAttachmentTransformRules::KeepWorldTransform);
	
	GetMovementComponent()->SetActive(false);
}

void ANSSpectator::EndModeAroundActor()
{
	GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCollisionComponent()->SetSimulatePhysics(true);
	
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GetMovementComponent()->SetActive(true);
}

void ANSSpectator::GetNextPlayerToAttach(bool bNext, bool bOnlyLive)
{
	if (const auto TeamManager = GetWorld()->GetGameState()->FindComponentByClass<UTeamSetupManager>())
	{
			//GetPlayerState() nullptr because we enter spectator mode incorrectly
		const auto TeamIndex = FGenericTeamId::GetTeamIdentifier(GetInstigatorController()->PlayerState).GetId();
		TeamManager -> GetNextPlayerInTeam(TeamIndex, CurrentAttachedPlayer, bNext, bOnlyLive);
	}
}

APawn* ANSSpectator::GetPawnToAttach()
{
	//get next actor to attach
	if (!CurrentAttachedPlayer)
		GetNextPlayerToAttach();
	if (!CurrentAttachedPlayer)	return nullptr;
	
	return  CurrentAttachedPlayer->GetPawn();
}
