// Fill out your copyright notice in the Description page of Project Settings.


#include "NSPlayerState.h"

#include "PCNetShooter.h"
#include "Net/UnrealNetwork.h"

void ANSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANSPlayerState, PlayerStatistic);
}

void ANSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	auto Controller = Cast<APCNetShooter>(GetInstigatorController());

	//server bind to kill someone
	//server bind to owning actor death
	//server asist
}

void ANSPlayerState::AddKill()
{
	PlayerStatistic.KillCount++;
}

void ANSPlayerState::AddAssist()
{
	PlayerStatistic.AssistCount++;
}

void ANSPlayerState::AddDeath()
{
	PlayerStatistic.DeathCount++;
}

void ANSPlayerState::OnRep_PlayerStatistic()
{
    // notify client about statitic update
	PlayerStatisticUpdateDelegate.Broadcast();
	UE_LOG(LogTemp, Error, TEXT("PlayerStatistic rep notify"))
}

//need update delegate on server side? (in add death and etc.)

