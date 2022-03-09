// Fill out your copyright notice in the Description page of Project Settings.


#include "NSPlayerStart.h"

#include "Components/CapsuleComponent.h"

bool ANSPlayerStart::CanSpawn(FName CommandName)
{
	auto Capsule = Cast<UCapsuleComponent>(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	//get owerlaping actors
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, APawn::StaticClass());
	
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//must be not any pawn
	if (OverlappingActors.IsValidIndex(0))
	{
		return false;
	}
	
	return true;
}
