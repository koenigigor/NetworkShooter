// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;
}

FGenericTeamId ATurret::GetGenericTeamId() const
{
	if (const auto TeamInterface = GetInstigator<IGenericTeamAgentInterface>())
		return TeamInterface->GetGenericTeamId();

	return FGenericTeamId(0);
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
