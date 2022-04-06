// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Turret.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

FGenericTeamId ATurret::GetGenericTeamId() const
{
	if (auto LInstigator = GetInstigator())
		if (auto TeamInterface = Cast<IGenericTeamAgentInterface>(LInstigator))
			return TeamInterface->GetGenericTeamId();

	return FGenericTeamId(0);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



