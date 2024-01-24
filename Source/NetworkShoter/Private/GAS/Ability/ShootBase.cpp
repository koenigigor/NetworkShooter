// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/ShootBase.h"

#include "DrawDebugHelpers.h"
#include "NSFunctionLibrary.h"
#include "Equipment/NSEquipmentInstance.h"
#include "GameFramework/PlayerState.h"
#include "GAS/AttributeSet/WeaponAttributeSet.h"

void UShootBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//ProcessShootQue(EQueStage::ActivateAbility); //see bAddShootCue
}

void UShootBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	//ProcessShootQue(EQueStage::EndAbility); //see bAddShootCue
}

FVector UShootBase::GetMuzzleLocation() const
{
	const auto WeaponActor = GetAssociatedEquipmentActor_Ensured();
	const auto AttachedTo = WeaponActor->GetRootComponent()->GetAttachParent();
	const auto WeaponOwner = AttachedTo->GetOwner<APawn>();
	const auto MuzzleLocation = WeaponActor->GetRootComponent()->GetSocketLocation("Muzzle"); //intend weapon mesh is root

	/*
	UE_LOG(LogTemp, Display, TEXT("WeaponActor = %s"), *WeaponActor->GetName())
	UE_LOG(LogTemp, Display, TEXT("AttachedTo = %s"), *AttachedTo->GetName())
	UE_LOG(LogTemp, Display, TEXT("AttachedToSocket = %s"), *AttachedToSocket.ToString())
	UE_LOG(LogTemp, Display, TEXT("WeaponOwner = %s"), *WeaponOwner->GetName())
	*/

	///up down look correction
	const auto OwnerToMuzzle = (MuzzleLocation - WeaponOwner->GetActorLocation()).GetAbs() * FVector(1,1,0);
	const auto RotatedMuzzleOffset = OwnerToMuzzle.Length() * WeaponOwner->GetViewRotation().Vector();
	const auto RotatedNuzzleLocation = WeaponOwner->GetActorLocation() + RotatedMuzzleOffset + FVector(0, 0, (MuzzleLocation - WeaponOwner->GetActorLocation()).GetAbs().Z);

	//DrawDebugPoint(GetWorld(), RotatedNuzzleLocation, 15, FColor::Magenta, false, 4, 1);

	return RotatedNuzzleLocation;
	
	//todo on +-45 degrees work ok, but more is not ok
	//todo height correction (crouch) 
	
}

void UShootBase::GetShootStartAndDirection(FVector& Start, FVector& Direction, float Length)
{
	Start = GetMuzzleLocation();
	
	FVector ViewEnd = UNSFunctionLibrary::GetActorViewPoint_NS(Cast<APawn>(GetAvatarActorFromActorInfo()), Length, GetTraceChannel());

	/*
	DrawDebugLine(GetWorld(), Start, ViewEnd, FColor::Red, false, 5.f, 0, 2);
	DrawDebugPoint(GetWorld(), Start, 3.f, FColor::Red, false, 5.f);
	DrawDebugPoint(GetWorld(), ViewEnd, 3.f, FColor::Yellow, false, 5.f);
	*/
	
	Direction = (ViewEnd - Start).GetSafeNormal();

	//DrawDebugDirectionalArrow(GetWorld(), Start, Start + Direction * 100.f,3.f, FColor::Green, false, 5.f);
}

void UShootBase::GetShootStartAndDirectionWithSpread(FVector& Start, FVector& Direction, float Length)
{
	GetShootStartAndDirection(Start, Direction, Length);

	const float SpreadPercent = 
		GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UWeaponAttributeSet::GetSpreadPercentAttribute());

	const float SpreadHalfAngle = FMath::GetRangeValue(FVector2f(Spread.X, Spread.Y), SpreadPercent);

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
	CollisionParams.bReturnPhysicalMaterial = true;
	
	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, GetTraceChannel(), CollisionParams);
}

void UShootBase::MakeShoot()
{
	ProcessShootQue(EQueStage::MakeShoot);

	for (int32 i = 0; i != BulletsPerShoot; ++i)
	{
		MakeSingleShoot();
	}
}

void UShootBase::MakeSingleShoot()
{
	
}

ECollisionChannel UShootBase::GetTraceChannel()
{
	return ECollisionChannel::ECC_GameTraceChannel2;
}

float UShootBase::GetShootDelay() const
{
	const float ShootsPerSec = 
	GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UWeaponAttributeSet::GetShootsPerSecAttribute());

	return FMath::IsNearlyZero(ShootsPerSec) ? 1.f : 1/ShootsPerSec;
}

FGameplayEffectSpecHandle UShootBase::MakeDamageEffectSpec() const
{
	const auto Instigator = GetAvatarActorFromActorInfo()->GetInstigator()->GetPlayerState();
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

void UShootBase::ProcessShootQue(EQueStage Stage) const
{
	if (Stage == EQueStage::EndAbility && bAddShootCue)
	{
		GetAbilitySystemComponentFromActorInfo_Checked() -> RemoveGameplayCue(ShootCueTag);
		return;
	}
	
	FGameplayCueParameters Parameters;
	Parameters.EffectCauser = GetAssociatedEquipmentActor();
	Parameters.SourceObject = GetAssociatedEquipment();
	Parameters.RawMagnitude = GetShootDelay();
	
	if (Stage == EQueStage::ActivateAbility && bAddShootCue)
	{
		GetAbilitySystemComponentFromActorInfo_Checked() -> AddGameplayCue(ShootCueTag, Parameters);
		return;
	}

	if (Stage == EQueStage::MakeShoot && !bAddShootCue)
	{
		GetAbilitySystemComponentFromActorInfo_Checked() -> ExecuteGameplayCue(ShootCueTag, Parameters);
		return;
	}
}
