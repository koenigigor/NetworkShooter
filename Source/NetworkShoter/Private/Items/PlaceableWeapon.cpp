// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PlaceableWeapon.h"

// Sets default values
APlaceableWeapon::APlaceableWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	//SetReplicatedMovement(FRepMovement);
}

bool APlaceableWeapon::CanPlace()
{
	return true;
	
	//or maybe GetDefaultObject<APawn>() it this disable collision
	bool bCanPlace = GetWorld()->EncroachingBlockingGeometry(this, GetActorLocation(), GetActorRotation());
	return bCanPlace;
}

void APlaceableWeapon::StartPlacingWeapon()
{
}

bool APlaceableWeapon::PlaceWeapon()
{
	if (CanPlace())
	{
		OnPlaced();
		return true;
	}
	return false;
}

void APlaceableWeapon::CancelPlacing()
{
	
}

void APlaceableWeapon::OnPlaced()
{
	SetActorEnableCollision(true);
	//set material
}



