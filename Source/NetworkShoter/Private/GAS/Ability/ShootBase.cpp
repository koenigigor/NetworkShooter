// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/ShootBase.h"

#include "DrawDebugHelpers.h"
#include "NSEquipment.h"
#include "NSFunctionLibrary.h"
#include "Items/Weapon.h"

void UShootBase::GetShootStartAndDirection(FVector& Start, FVector& Direction, float Length)
{
	UNSEquipment* Equipment = GetOwningActorFromActorInfo()->FindComponentByClass<UNSEquipment>();
	ensure(Equipment && Equipment->GetEquippedWeapon());
	Start = Equipment->GetEquippedWeapon()->GetRootComponent()->GetSocketLocation("Muzzle");
	
	FVector ViewEnd = UNSFunctionLibrary::GetActorViewPoint_NS(GetAvatarActorFromActorInfo(), Length, GetTraceChannel());

	/*
	DrawDebugLine(GetWorld(), Start, ViewEnd, FColor::Red, false, 20.f, 0, 2);
	DrawDebugPoint(GetWorld(), Start, 3.f, FColor::Red, false, 20.f);
	DrawDebugPoint(GetWorld(), ViewEnd, 3.f, FColor::Yellow, false, 20.f);
	*/
	
	Direction = (ViewEnd - Start).GetSafeNormal();

	DrawDebugDirectionalArrow(GetWorld(), Start, Start + Direction * 100.f,3.f, FColor::Green, false, 20.f);
}

void UShootBase::GetShootStartAndDirectionWithSpread(FVector& Start, FVector& Direction, float Length)
{
	GetShootStartAndDirection(Start, Direction, Length);

	auto Weapon = GetAssociatedWeapon();
	check(Weapon);

	auto WeaponData = Weapon->WeaponData;
	check(WeaponData)

	Direction = UNSFunctionLibrary::GetRandConeNormalDistribution(Direction, WeaponData->SpreadHalfAngle, WeaponData->SpreadExponent);
}

void UShootBase::MakeHit(FHitResult& OutHit)
{
	const float ShootDistance = 1000.f; //todo weapon attribute

	FVector Start;
	FVector Direction;
	GetShootStartAndDirectionWithSpread(Start, Direction, ShootDistance);

	FVector End = Start + (Direction * ShootDistance);
	
	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, GetTraceChannel());
}

AWeapon* UShootBase::GetAssociatedWeapon()
{
	if (auto Spec = GetCurrentAbilitySpec())
	{
		return Cast<AWeapon>(Spec->SourceObject);
	}
	
	return nullptr;
}

ECollisionChannel UShootBase::GetTraceChannel()
{
	return ECollisionChannel::ECC_GameTraceChannel2;
}

FGameplayEffectSpecHandle UShootBase::MakeDamageEffectSpec()
{
	const auto Instigator = GetAvatarActorFromActorInfo();
	const auto Causer = GetAssociatedWeapon();

	if (!IsValid(DamageEffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid damage effect in shoot ability"))
		return FGameplayEffectSpecHandle();
	}
	
	const auto EffectSpec = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	EffectSpec.Data.Get()->GetContext().AddInstigator(Instigator, Causer);

	return EffectSpec;
}
