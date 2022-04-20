// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/NSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ANSProjectile::ANSProjectile()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	RootComponent = SphereCollision;
}


void ANSProjectile::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->IgnoreActorWhenMoving(GetOwner(), true);
	SphereCollision->IgnoreActorWhenMoving(GetInstigator(), true);
}


