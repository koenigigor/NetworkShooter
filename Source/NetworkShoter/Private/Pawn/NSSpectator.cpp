// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/NSSpectator.h"

#include "Game/NSGameState.h"

// Sets default values
ANSSpectator::ANSSpectator()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttachedActor.Key = nullptr;
	AttachedActor.Value = -1;
}

// Called when the game starts or when spawned
void ANSSpectator::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANSSpectator::SetSpectatorMode(ESpectatorMode Mode)
{
	//exit from current mode
	
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

void ANSSpectator::SetModeAttachToActor()
{
	//get actors list from game state
	AActor* ActorToAttach = nullptr;
	//todo
	if (auto NSGameState = Cast<ANSGameState>(GetWorld()->GetGameState()))
	{
		//TODO team
		NSGameState -> GetNextActorInTeam("Team1", AttachedActor.Key, AttachedActor.Value);
		ActorToAttach = AttachedActor.Key;
	}
	
	if (!ActorToAttach)
	{
		UE_LOG(LogTemp, Warning, TEXT("ANSSpectator::SetModeAttachToActor nothing to attach"))
		return;
	}
	
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
	//todo
}

// Called every frame
void ANSSpectator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANSSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

AActor* ANSSpectator::GetAttachedActor()
{
	return AttachedActor.Key;
}

