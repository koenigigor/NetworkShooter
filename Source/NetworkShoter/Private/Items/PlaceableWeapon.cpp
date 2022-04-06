// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PlaceableWeapon.h"

#include "NSFunctionLibrary.h"

// Sets default values
APlaceableWeapon::APlaceableWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(false);

	bReplicates = true;
	//SetReplicatedMovement(FRepMovement);
}

void APlaceableWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckCanBePlaced();
	UpdatePlaceLocation();
}

void APlaceableWeapon::ToggleMaterial(bool bNormal)
{
	BP_ToggleMaterial(bNormal);
}

void APlaceableWeapon::FinishPlaceWeapon_Implementation()
{
	SetActorEnableCollision(true);

	//spawn actor
	if (SpawnedActor)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator = GetInstigator();
		SpawnParameters.Owner = GetOwner();
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(SpawnedActor, GetTransform(), SpawnParameters);
	}

	BP_FinishPlaceWeapon();

	//destroy self
	Destroy();
}

bool APlaceableWeapon::CanPlace()
{
	FVector BoxHalfExtent;
	FVector Origin;
	GetActorBounds(false, Origin, BoxHalfExtent);
	
	TArray<FOverlapResult> OutOverlaps;
	FVector Position = Origin + FVector(0,0,25); //up
	
	bool bOverlap = GetWorld() -> OverlapBlockingTestByChannel(
		Position,
		GetActorQuat(),
		ECC_GameTraceChannel2, //WeaponChanel
		FCollisionShape::MakeBox(BoxHalfExtent));
	
	return !bOverlap;
}

void APlaceableWeapon::CheckCanBePlaced()
{
	bool bCanPlace = CanPlace();
	if (bCanPlace == bCanPlaceOnPreviousFrame) return;

	ToggleMaterial(bCanPlace);

	bCanPlaceOnPreviousFrame = bCanPlace;
}

void APlaceableWeapon::UpdatePlaceLocation()
{
	FHitResult Hit;
	FVector Start = UNSFunctionLibrary::GetActorViewPoint_NS(GetOwner(), 400, ECC_Camera);
	FVector End = Start + FVector::DownVector * 500;
		
	auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera);

	/*
	DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 3.f, 0, 2);
	DrawDebugPoint(GetWorld(), Start, 10.f, FColor::Cyan, false, 3.f);
	DrawDebugPoint(GetWorld(), End, 10.f, FColor::Green, false, 3.f);
	*/
		
	FVector Ground = bHit ? Hit.ImpactPoint : Hit.TraceEnd;

	SetActorLocation(Ground);
		
	SetActorRotation(FRotator(0, GetInstigator()->GetViewRotation().Yaw, 0));
}

