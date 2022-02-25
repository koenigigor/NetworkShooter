// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask/AbilityTask_PlayerShootTrace.h"

#include "DrawDebugHelpers.h"
#include "NSEquipment.h"
#include "NetworkShoter/Weapon.h"

void UAbilityTask_PlayerShootTrace::Activate()
{
	Super::Activate();

	if (ShootDistantion <= 0) { return; }

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActor());
	
	//Get point where character view
	FVector ViewStart;
	FRotator ViewRotation;
	FHitResult ViewHit;
	GetOwnerActor()->GetInstigator()->GetController()->GetActorEyesViewPoint(ViewStart, ViewRotation);
	FVector ViewEnd = ViewRotation.Vector() * ShootDistantion;
	
	//ECC_GameTraceChannel2 weapon channel
	GetWorld()->LineTraceSingleByChannel(ViewHit, ViewStart, ViewEnd, ECollisionChannel::ECC_GameTraceChannel2, QueryParams);
	
	//Trace from weapon
	UNSEquipment* Equipment = GetOwnerActor()->FindComponentByClass<UNSEquipment>();
	ensure(Equipment && Equipment->GetEquippedWeapon());
	FVector WeaponStart = Equipment->GetEquippedWeapon()->GetRootComponent()->GetSocketLocation("Muzzle");
	FVector WeaponEnd = ViewHit.bBlockingHit ? ViewHit.ImpactPoint : ViewHit.TraceEnd;
	FHitResult WeaponHit;

	GetWorld()->LineTraceSingleByChannel(WeaponHit, WeaponStart, WeaponEnd, ECollisionChannel::ECC_GameTraceChannel2, QueryParams);
	
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
