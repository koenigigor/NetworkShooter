// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/ShootProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Items/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Items/NSProjectile.h"

void UShootProjectile::MakeShoot()
{
	Super::MakeShoot();

	const auto Velocity = SuggestProjectileVelocity();
	SpawnProjectile(Velocity);
}

FVector UShootProjectile::SuggestProjectileVelocity()
{
	if (!GetAssociatedWeapon())
		return FVector::ZeroVector;
	
	FHitResult Hit;
	MakeHit(Hit);

	const FVector StartPoint = Hit.TraceStart;
	const FVector EndPoint = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;
	const float Speed = GetAssociatedWeapon()->WeaponData->ProjectileSpeed;

	FVector Velocity;
	
	bool bSolutionFound = UGameplayStatics::SuggestProjectileVelocity(GetWorld(), Velocity, StartPoint, EndPoint, Speed,
		false, 6.f, 0, ESuggestProjVelocityTraceOption::DoNotTrace);

	if (!bSolutionFound)
	{
		Velocity = (EndPoint - StartPoint) * Hit.Distance;
	}

	return Velocity;
}

ECollisionChannel UShootProjectile::GetTraceChannel()
{
	return ECC_GameTraceChannel3; //projectile channel
}

void UShootProjectile::SpawnProjectile(FVector Velocity)
{
	auto ProjectileClass = GetAssociatedWeapon()->WeaponData->ProjectileClass;
	FTransform SpawnTransform = GetAssociatedWeapon()->GetRootComponent()->GetSocketTransform("Muzzle");

	auto SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ProjectileClass, FTransform::Identity, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, GetAvatarActorFromActorInfo());

	auto Projectile = StaticCast<ANSProjectile*>(SpawnedActor);
	Projectile->DamageEffectHandle = MakeDamageEffectSpec();
	Projectile->SetInstigator(Cast<APawn>(GetAvatarActorFromActorInfo()));
	Projectile->ProjectileMovement->Velocity = Velocity;
	
	UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnTransform);
}
