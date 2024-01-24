// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/ShootProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Items/NSProjectile.h"

void UShootProjectile::MakeSingleShoot()
{
	Super::MakeSingleShoot();

	const auto Velocity = SuggestProjectileVelocity();
	SpawnProjectile(Velocity);
}

FVector UShootProjectile::SuggestProjectileVelocity()
{
	FHitResult Hit;
	MakeHit(Hit);

	const FVector StartPoint = Hit.TraceStart;
	const FVector EndPoint = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;

	FVector Velocity;
	
	bool bSolutionFound = UGameplayStatics::SuggestProjectileVelocity(
		GetWorld(), Velocity, StartPoint, EndPoint, ProjectileSpeed,
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
	auto SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), ProjectileClass, FTransform::Identity, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, GetAvatarActorFromActorInfo());

	const auto Projectile = StaticCast<ANSProjectile*>(SpawnedActor);
	Projectile->DamageEffectHandle = MakeDamageEffectSpec();
	Projectile->ImpactCue = ImpactCue;
	Projectile->SetInstigator(Cast<APawn>(GetAvatarActorFromActorInfo()));
	Projectile->Velocity = Velocity;
	Projectile->SourceASC = GetAbilitySystemComponentFromActorInfo();
	
	UGameplayStatics::FinishSpawningActor(SpawnedActor, FTransform(GetMuzzleLocation()));
}
