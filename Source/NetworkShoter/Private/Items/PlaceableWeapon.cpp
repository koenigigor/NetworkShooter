// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PlaceableWeapon.h"

// Sets default values
APlaceableWeapon::APlaceableWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	//SetReplicatedMovement(FRepMovement);
}

void APlaceableWeapon::ToggleMaterial(bool bNormal)
{
	BP_ToggleMaterial(bNormal);
}

void APlaceableWeapon::FinishPlaceWeapon_Implementation()
{
	SetActorEnableCollision(true);

	BP_FinishPlaceWeapon();
}



