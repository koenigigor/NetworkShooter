// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PlaceableWeaponStorage.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
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
	DOREPLIFETIME(UPlaceableWeaponStorage, PlacedWeapon);
	DOREPLIFETIME(UPlaceableWeaponStorage, Arm);
}

void UPlaceableWeaponStorage::StartPlaceWeapon_Implementation()
{
	//get stored weapon
	if (!StoredWeaponClass) {return;}
	if (PlacedWeapon) {return;}

	//spawn weapon
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = Cast<APawn>(GetOwner());
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PlacedWeapon = GetWorld()->SpawnActor<APlaceableWeapon>(StoredWeaponClass, FVector(0), FRotator(0), SpawnParameters);
    PlacedWeapon -> SetActorEnableCollision(false);


	//instead line trace for get player view, use spring arm for get more flexibility in future
	Arm = NewObject<USpringArmComponent>(GetOwner());
	Arm->ProbeChannel = ECollisionChannel::ECC_Camera;
	Arm->bDoCollisionTest = true;
	Arm->TargetArmLength = -400;
	Arm->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Arm->RegisterComponent();
	Arm->bUsePawnControlRotation = true;
}

void UPlaceableWeaponStorage::FinishPlaceWeapon()
{
	if (!PlacedWeapon->CanPlace()) return;
	
	Arm->DestroyComponent();
	Arm=nullptr;
	
	PlacedWeapon->PlaceWeapon();
	StoredWeaponClass = nullptr;
	PlacedWeapon = nullptr;
}

void UPlaceableWeaponStorage::CancelPlaceWeapon()
{
	Arm->DestroyComponent();
	Arm=nullptr;
	
	PlacedWeapon->Destroy();
	PlacedWeapon=nullptr;
}


void UPlaceableWeaponStorage::UpdatePlaceLocation()
{
	if (Arm && PlacedWeapon)
	{
		FHitResult Hit;
		FVector Start = Arm->GetSocketLocation(Arm->SocketName);
		
		FVector End = Start + FVector::DownVector * 500;
		auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera);

		/*
		DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 3.f, 0, 2);
		DrawDebugPoint(GetWorld(), Start, 10.f, FColor::Cyan, false, 3.f);
		DrawDebugPoint(GetWorld(), End, 10.f, FColor::Green, false, 3.f);
		*/
		
		FVector Ground = bHit ? Hit.ImpactPoint : Hit.TraceEnd;

		PlacedWeapon->SetActorLocation(Ground);
		
		//inherit spring arm rotation
		PlacedWeapon->SetActorRotation(FRotator(0, Arm->GetDesiredRotation().Yaw, 0));
	}
}

void UPlaceableWeaponStorage::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdatePlaceLocation();	
}

