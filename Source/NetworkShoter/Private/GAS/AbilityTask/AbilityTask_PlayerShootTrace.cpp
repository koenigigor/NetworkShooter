// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/AbilityTask_PlayerShootTrace.h"

#include "DrawDebugHelpers.h"
#include "NSEquipment.h"
#include "Camera/CameraComponent.h"
#include "Items/Weapon.h"

void UAbilityTask_PlayerShootTrace::Activate()
{
	Super::Activate();

	if (ShootDistantion <= 0) { return; }

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwnerActor());
	QueryParams.AddIgnoredActor(GetAvatarActor());
	
	//Get point where character view
	FVector ViewStart;
	FHitResult ViewHit;
	
	FRotator ViewRotation;
	GetOwnerActor()->GetInstigator()->GetController()->GetActorEyesViewPoint(ViewStart, ViewRotation);
	
	FVector ViewEnd = ViewRotation.Vector() * ShootDistantion + ViewStart;
	
	//ECC_GameTraceChannel2 weapon channel
	GetWorld()->LineTraceSingleByChannel(ViewHit, ViewStart, ViewEnd, ECollisionChannel::ECC_GameTraceChannel2, QueryParams);

	/*
	DrawDebugLine(GetWorld(), ViewStart, ViewEnd, FColor::Red, false, 20.f, 0, 2);
	DrawDebugPoint(GetWorld(), ViewStart, 3.f, FColor::Red, false, 20.f);
	DrawDebugPoint(GetWorld(), ViewEnd, 3.f, FColor::Red, false, 20.f);
	*/
	
	//Trace from weapon
	UNSEquipment* Equipment = GetOwnerActor()->FindComponentByClass<UNSEquipment>();
	ensure(Equipment && Equipment->GetEquippedWeapon());
	FVector WeaponStart = Equipment->GetEquippedWeapon()->GetRootComponent()->GetSocketLocation("Muzzle");
	FVector WeaponEnd = ViewHit.bBlockingHit ? ViewHit.ImpactPoint + (ViewRotation.Vector()*5.f) : ViewHit.TraceEnd;
		//(ViewRotation.Vector()*5.f) fix when weapon trace not long enough
	FHitResult WeaponHit;

	GetWorld()->LineTraceSingleByChannel(WeaponHit, WeaponStart, WeaponEnd, ECollisionChannel::ECC_GameTraceChannel2, QueryParams);

	/*
	DrawDebugLine(GetWorld(), WeaponStart, WeaponEnd, FColor::Cyan, false, 20.f, 0, 2);
	DrawDebugPoint(GetWorld(), WeaponStart, 10.f, FColor::Cyan, false, 20.f);
	DrawDebugPoint(GetWorld(), WeaponEnd, 10.f, FColor::Green, false, 20.f);
	*/
	
	// Construct TargetData
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(WeaponHit);

	// Give it a handle and return
	FGameplayAbilityTargetDataHandle Handle;
	Handle.Data.Add(TSharedPtr<FGameplayAbilityTargetData>(TargetData));

	WaitShootResult.Broadcast(Handle);
}

UAbilityTask_PlayerShootTrace* UAbilityTask_PlayerShootTrace::AbilityTask_PlayerShootTrace(UGameplayAbility* OwningAbility, float Distantion)
{
	auto Task = NewAbilityTask<UAbilityTask_PlayerShootTrace>(OwningAbility);
	Task->ShootDistantion = Distantion;
	return Task;
}
