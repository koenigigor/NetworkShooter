// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PlaceableWeapon.h"

#include "NSFunctionLibrary.h"
#include "Abilities/GameplayAbility.h"

void APlaceableWeapon::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SetOwner(Ability->GetAvatarActorFromActorInfo());
	SetInstigator(Cast<APawn>(Ability->GetAvatarActorFromActorInfo()));
}

bool APlaceableWeapon::IsConfirmTargetingAllowed()
{
	return CanPlace();
}

void APlaceableWeapon::ConfirmTargetingAndContinue()
{
	if (IsConfirmTargetingAllowed())
	{
		FinishPlaceWeapon();
		
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
	}
}

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

bool APlaceableWeapon::FinishPlaceWeapon()
{
	if (!CanPlace())
		return false;

	//spawn actor
	if (SpawnedActor)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator = GetInstigator();
		SpawnParameters.Owner = GetOwner();
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(SpawnedActor, GetTransform(), SpawnParameters);
	}

	FinishPlaceNotify();

	//destroy self
	Destroy();

	return true;
}

void APlaceableWeapon::FinishPlaceNotify_Implementation()
{
	BP_FinishPlaceWeapon();
}

bool APlaceableWeapon::CanPlace()
{
	FVector BoxHalfExtent;
	FVector Origin;
	GetActorBounds(false, Origin, BoxHalfExtent);
	
	TArray<FOverlapResult> OutOverlaps;
	FVector Position = Origin + FVector(0,0,25); //up

	/*
	UE_LOG(LogTemp, Display, TEXT("Place pos = %s"), *Position.ToString());	
	DrawDebugBox(
		GetWorld(),
		Position, BoxHalfExtent,
		FColor::Silver,
		false, 0.2f, 0, 1.f
		);
	*/
	
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
	if (!GetOwner()) return;
	
	FHitResult Hit;
	FVector Start = UNSFunctionLibrary::GetActorViewPoint_NS(GetOwner<APawn>(), 400, ECC_Camera)+ FVector(0.f, 0.f, 30.f);
	FVector End = Start + FVector::DownVector * 500;
		
	auto bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera);

	/*
	DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 3.f, 0, 2);
	DrawDebugPoint(GetWorld(), Start, 10.f, FColor::Cyan, false, 3.f);
	DrawDebugPoint(GetWorld(), End, 10.f, FColor::Green, false, 3.f);
	*/
		
	FVector Ground = bHit ? Hit.ImpactPoint : Hit.TraceEnd;

	SetActorLocation(Ground);

	if (!GetInstigator()) return;;
	
	SetActorRotation(FRotator(0, GetInstigator()->GetViewRotation().Yaw, 0));
}

