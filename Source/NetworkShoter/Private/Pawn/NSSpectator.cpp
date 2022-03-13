// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/NSSpectator.h"

// Sets default values
ANSSpectator::ANSSpectator()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
}

void ANSSpectator::SetModeAttachToActor()
{
	//get actors list from game state
	AActor* ActorToAttach = nullptr;
	//todo
	
	//Set actor camera view
	if (GetController())
	{
		if (auto PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerController -> SetViewTarget(ActorToAttach);
		}
	}
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

