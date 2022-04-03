// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PlaceableWeaponStorage.h"

#include "DrawDebugHelpers.h"
#include "NSFunctionLibrary.h"
#include "Items/PlaceableWeapon.h"
#include "Net/UnrealNetwork.h"


UPlaceableWeaponStorage::UPlaceableWeaponStorage()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UPlaceableWeaponStorage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlaceableWeaponStorage, WeaponToPlace);
}

void UPlaceableWeaponStorage::BeginPlay()
{
	Super::BeginPlay();

	OwningPawn = Cast<APawn>(GetOwner());
}

bool UPlaceableWeaponStorage::IsInPlacingMode()
{
	return WeaponToPlace ? true : false;
}

void UPlaceableWeaponStorage::StartPlaceWeapon_Implementation()
{
	//get stored weapon
	if (!StoredWeaponClass) {return;}
	if (IsInPlacingMode()) {return;} 

	//spawn weapon
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = OwningPawn;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	WeaponToPlace = GetWorld()->SpawnActor<APlaceableWeapon>(StoredWeaponClass, FVector(0), FRotator(0), SpawnParameters);
	OnRep_WeaponToPlace();
}

void UPlaceableWeaponStorage::FinishPlaceWeapon_Implementation()
{
	if (!CanPlace()) return;
	
	WeaponToPlace->FinishPlaceWeapon();
	StoredWeaponClass = nullptr;
	WeaponToPlace = nullptr;
}

void UPlaceableWeaponStorage::CancelPlaceWeapon_Implementation()
{
	WeaponToPlace->Destroy();
	WeaponToPlace=nullptr;
}

void UPlaceableWeaponStorage::UpdatePlaceLocation()
{
	if (WeaponToPlace)
	{
		FHitResult Hit;
		FVector Start = UNSFunctionLibrary::GetActorViewPoint_NS(OwningPawn, 400, ECC_Camera);
		FVector End = Start + FVector::DownVector * 500;
		
		auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera);

		/*
		DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 3.f, 0, 2);
		DrawDebugPoint(GetWorld(), Start, 10.f, FColor::Cyan, false, 3.f);
		DrawDebugPoint(GetWorld(), End, 10.f, FColor::Green, false, 3.f);
		*/
		
		FVector Ground = bHit ? Hit.ImpactPoint : Hit.TraceEnd;

		WeaponToPlace->SetActorLocation(Ground);
		
		WeaponToPlace->SetActorRotation(FRotator(0, OwningPawn->GetViewRotation().Yaw, 0));
	}
}

void UPlaceableWeaponStorage::CheckCanBePlaced()
{
	bool bCanPlace = CanPlace();
	if (bCanPlace == bCanPlaceOnPreviousFrame) return;

	WeaponToPlace -> ToggleMaterial(bCanPlace);

	bCanPlaceOnPreviousFrame = bCanPlace;
}

bool UPlaceableWeaponStorage::CanPlace()
{
	if (!WeaponToPlace) return false;
	
	FVector BoxHalfExtent;
	FVector Origin;
	WeaponToPlace->GetActorBounds(false, Origin, BoxHalfExtent);
	
	TArray<FOverlapResult> OutOverlaps;
	FVector Position = Origin + FVector(0,0,25); //up
	
	bool bOverlap = GetWorld() -> OverlapBlockingTestByChannel(
		Position,
		WeaponToPlace->GetActorQuat(),
		ECC_GameTraceChannel2, //WeaponChanel
		FCollisionShape::MakeBox(BoxHalfExtent));

	//DrawDebugBox(GetWorld(), Position, BoxHalfExtent, WeaponToPlace->GetActorQuat(), FColor::Blue);
	
	return !bOverlap;
}

void UPlaceableWeaponStorage::OnRep_WeaponToPlace()
{
	if (!WeaponToPlace) return;

	WeaponToPlace->SetActorEnableCollision(false);
}

void UPlaceableWeaponStorage::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsInPlacingMode())
	{
		UpdatePlaceLocation();
		CheckCanBePlaced();
	}
}

