// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/NSProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueFunctionLibrary.h"
#include "GameplayCueManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


ANSProjectile::ANSProjectile()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	SphereCollision->bReturnMaterialOnMove = true;
	RootComponent = SphereCollision;

	bReplicates = true;
	SetReplicatingMovement(false);
}

void ANSProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ImpactCue, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, Velocity, COND_InitialOnly);
}


void ANSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMovement->Velocity = Velocity;

	SphereCollision->IgnoreActorWhenMoving(GetOwner(), true);
	SphereCollision->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ANSProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	if (HasAuthority())
	{
		//apply gameplay effect
		if (const auto TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Other))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectHandle.Data.Get());
		}
		
		//Destroy(); //wait for client also hit
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetLifeSpan(1.f);
	}
	else 
	{
		if (ImpactCue.IsValid() && Hit.bBlockingHit)
		{
			UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetInstigator(), ImpactCue, UGameplayCueFunctionLibrary::MakeGameplayCueParametersFromHitResult(Hit));
		}

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		//Destroy(); //NO probably actor replicates again? in different location and make a lot of hts
	}
}

