// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretRotation.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


UTurretRotation::UTurretRotation()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTurretRotation::PostInitProperties()
{
	Super::PostInitProperties();

	SetComponentTickEnabled(false);
}

void UTurretRotation::Init(USceneComponent* Turret, USceneComponent* Barrel)
{
	TurretRef=Turret;
	BarrelRef=Barrel;
}

void UTurretRotation::SetTarget(AActor* NewTarget)
{
	if (!NewTarget)
	{
		Target=NewTarget; //clear previous target if was been set
		SetComponentTickEnabled(false);
	}
		
	if (!TurretRef && !BarrelRef)
	{
		UE_LOG(LogTemp, Error, TEXT("UTurretRotation not set referenses in actor %s"), *GetOwner()->GetName())
		return;
	}

	Target = NewTarget;
	SetComponentTickEnabled(true);
}

void UTurretRotation::UpdateResultRotation()
{
	if (!Target) return;

	FVector BoxExtent;
	FVector CenterTarget;
	Target->GetActorBounds(true, CenterTarget, BoxExtent);
	
	if (!bHasProjectile)
	{
		FVector Direction = CenterTarget - GetOwner()->GetActorLocation();
		ResultRotation = Direction.ToOrientationRotator();

		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(),
			CenterTarget, FColor::Blue);
		
		return;
	}
	
	if (bHasProjectile)
	{
		FVector End = CenterTarget;
		FVector Start = BarrelRef ? BarrelRef -> GetSocketLocation("Muzzle") : GetOwner()->GetActorLocation();

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());
		
		FVector LaunchVelocity;
		
		bool bSolutionFound = UGameplayStatics::SuggestProjectileVelocity(this,
			LaunchVelocity,
			Start, End,
			ProjectileSpeed,
			false,
			2.f,0.f,
			ESuggestProjVelocityTraceOption::DoNotTrace,
			FCollisionResponseParams::DefaultResponseParam,
			IgnoreActors,
		false);

		if (bSolutionFound)
		{
			ResultRotation = LaunchVelocity.ToOrientationRotator();
		}
	}
}

void UTurretRotation::RotateTurret()
{
	if (!TurretRef) return;
	
	float TurretRotation = TurretRef -> GetComponentRotation().Yaw;
	float ResultTurretRotation = FMath::Clamp<float>(ResultRotation.Yaw, TurretMinAngle, TurretMaxAngle);
	
	//Calculate rotation offset
	float TurretOffset = ResultTurretRotation - TurretRotation;
	TurretOffset = FMath::Wrap(TurretOffset, -180.f, 180.f);
	
	//get rotation offset for this frame
	auto Offset = FMath::FInterpConstantTo(
		0.f,
		TurretOffset,
		GetWorld()-> DeltaTimeSeconds,
		TurretRotationSpeed);

	//apply rotation
	TurretRef -> AddRelativeRotation(FRotator(0.f, Offset, 0.f));
}

void UTurretRotation::RotateBarrel()
{
	if (!BarrelRef) return;

	float BarrelRotation = BarrelRef -> GetComponentRotation().Pitch;
	float ResultBarrelRotation = FMath::Clamp<float>(ResultRotation.Pitch, BarrelMinAngle, BarrelMaxAngle);
	
	//Calculate rotation offset
	float BarrelOffset = ResultBarrelRotation - BarrelRotation;
	BarrelOffset = FMath::Wrap(BarrelOffset, -180.f, 180.f);
	
	//get rotation offset for this frame
	auto Offset = FMath::FInterpConstantTo(
		0.f,
		BarrelOffset,
		GetWorld()-> DeltaTimeSeconds,
		BarrelRotationSpeed);

	//apply rotation
	BarrelRef -> AddRelativeRotation(FRotator(Offset, 0.f, 0.f));
}


void UTurretRotation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateResultRotation();
	RotateTurret();
	RotateBarrel();
}

