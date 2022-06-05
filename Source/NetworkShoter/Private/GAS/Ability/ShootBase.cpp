// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/ShootBase.h"

#include "DrawDebugHelpers.h"
#include "NSFunctionLibrary.h"
#include "Equipment/NSEquipmentInstance.h"
#include "Items/Weapon.h"

FVector UShootBase::GetMuzzleLocation() const
{
	FVector DebugPoint;

	UE_LOG(LogTemp, Display, TEXT("UShootBase::GetMuzzleLocation %s"), *GetName())
	
	const auto WeaponActor = GetAssociatedEquipment()->SpawnedActors[0];
	const auto AttachedTo = WeaponActor->GetRootComponent()->GetAttachParent();
	const auto AttachedToSocket = WeaponActor->GetAttachParentSocketName();
	const auto WeaponOwner = Cast<APawn>(AttachedTo->GetOwner());
	const auto MuzzleLocation = WeaponActor->GetRootComponent()->GetSocketLocation("Muzzle"); //intend weapon mesh is root

	/*
	UE_LOG(LogTemp, Display, TEXT("WeaponActor = %s"), *WeaponActor->GetName())
	UE_LOG(LogTemp, Display, TEXT("AttachedTo = %s"), *AttachedTo->GetName())
	UE_LOG(LogTemp, Display, TEXT("AttachedToSocket = %s"), *AttachedToSocket.ToString())
	UE_LOG(LogTemp, Display, TEXT("WeaponOwner = %s"), *WeaponOwner->GetName())
	*/

	///up down look correction
	//const auto OwnerToMuzzle = (WeaponOwner->GetActorLocation() - MuzzleLocation) * FVector(1,1,0);
	const auto OwnerToMuzzle = (MuzzleLocation - WeaponOwner->GetActorLocation()).GetAbs() * FVector(1,1,0);
	const auto ViewPitchAxis =  FRotator(WeaponOwner->GetViewRotation().Pitch, 0,0);
	const auto OwnerToMuzzleRotated = ViewPitchAxis.RotateVector(OwnerToMuzzle);
	

	const auto RotatedLength = OwnerToMuzzle.Length() * WeaponOwner->GetViewRotation().Vector();
	DebugPoint = WeaponOwner->GetActorLocation() + RotatedLength + FVector(0, 0, (MuzzleLocation - WeaponOwner->GetActorLocation()).GetAbs().Z);
	DrawDebugPoint(GetWorld(), DebugPoint, 15, FColor::Magenta, false, 4, 1);
	return DebugPoint;

	UE_LOG(LogTemp, Display, TEXT("OwnerToMuzzle = %s"), *OwnerToMuzzle.ToString())
	UE_LOG(LogTemp, Display, TEXT("ViewPitchAxis = %s"), *ViewPitchAxis.ToString())
	UE_LOG(LogTemp, Display, TEXT("OwnerToMuzzleRotated = %s"), *OwnerToMuzzleRotated.ToString())
	
	const auto MuzzleLocationRotated = MuzzleLocation - OwnerToMuzzle + OwnerToMuzzleRotated;
	UE_LOG(LogTemp, Display, TEXT("MuzzleLocationRotated = %s"), *MuzzleLocationRotated.ToString())

	DebugPoint = MuzzleLocationRotated;
	//DrawDebugPoint(GetWorld(), DebugPoint, 15, FColor::Magenta, false, 4, 1);

	//todo on +-45 degrees work ok, but more is not ok
	//todo height correction (crouch) 
	
	return MuzzleLocationRotated;
}

void UShootBase::GetShootStartAndDirection(FVector& Start, FVector& Direction, float Length)
{
	Start = GetMuzzleLocation();
	
	FVector ViewEnd = UNSFunctionLibrary::GetActorViewPoint_NS(GetAvatarActorFromActorInfo(), Length, GetTraceChannel());

	
	DrawDebugLine(GetWorld(), Start, ViewEnd, FColor::Red, false, 5.f, 0, 2);
	DrawDebugPoint(GetWorld(), Start, 3.f, FColor::Red, false, 5.f);
	DrawDebugPoint(GetWorld(), ViewEnd, 3.f, FColor::Yellow, false, 5.f);
	
	
	Direction = (ViewEnd - Start).GetSafeNormal();

	DrawDebugDirectionalArrow(GetWorld(), Start, Start + Direction * 100.f,3.f, FColor::Green, false, 5.f);
}

void UShootBase::GetShootStartAndDirectionWithSpread(FVector& Start, FVector& Direction, float Length)
{
	GetShootStartAndDirection(Start, Direction, Length);

	const float SpreadPercent = 
		GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UWeaponAttributeSet::GetSpreadPercentAttribute());

	const float SpreadHalfAngle = FMath::GetRangeValue(FVector2f(SpreadMin, SpreadMax), SpreadPercent);

	Direction = UNSFunctionLibrary::GetRandConeNormalDistribution(Direction, SpreadHalfAngle, SpreadExponent);
}

void UShootBase::MakeHit(FHitResult& OutHit)
{
	FVector Start;
	FVector Direction;
	GetShootStartAndDirectionWithSpread(Start, Direction, ShootDistance);

	FVector End = Start + (Direction * ShootDistance);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
	
	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, GetTraceChannel(), CollisionParams);
}

ECollisionChannel UShootBase::GetTraceChannel()
{
	return ECollisionChannel::ECC_GameTraceChannel2;
}

FGameplayEffectSpecHandle UShootBase::MakeDamageEffectSpec()
{
	const auto Instigator = GetAvatarActorFromActorInfo();
	ensure(GetAssociatedEquipment()->SpawnedActors.IsValidIndex(0));
	const auto Causer = GetAssociatedEquipment()->SpawnedActors[0]; //mb remove array and make single spawned actor

	if (!IsValid(DamageEffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid damage effect in shoot ability"))
		return FGameplayEffectSpecHandle();
	}
	
	const auto EffectSpec = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	EffectSpec.Data.Get()->GetContext().AddInstigator(Instigator, Causer);

	return EffectSpec;
}
