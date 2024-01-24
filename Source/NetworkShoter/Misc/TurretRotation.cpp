// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretRotation.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogTurretRotation, All, All);

UTurretRotation::UTurretRotation()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTurretRotation::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
}

void UTurretRotation::Init(USceneComponent* Turret, USceneComponent* Barrel)
{
	TurretRef = Turret;
	BarrelRef = Barrel;
}

void UTurretRotation::SetTarget(AActor* NewTarget)
{
	if (!NewTarget)
	{
		Target = nullptr;
		SetComponentTickEnabled(false);
	}

	if (!TurretRef || !BarrelRef)
	{
		UE_LOG(LogTurretRotation, Warning, TEXT("Target set, but not initialise reference on turret in %s"), *GetOwner()->GetName())
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
		const auto Direction = CenterTarget - GetOwner()->GetActorLocation();
		ResultRotation = Direction.ToOrientationRotator();

		if (bDrawDebug) DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), CenterTarget, FColor::Blue);

		return;
	}

	if (bHasProjectile)
	{
		const auto Start = BarrelRef ? BarrelRef->GetSocketLocation("Muzzle") : GetOwner()->GetActorLocation();
		const auto End = CenterTarget;

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());

		FVector LaunchVelocity;

		const auto bSolutionFound = UGameplayStatics::SuggestProjectileVelocity(this,
			LaunchVelocity,
			Start, End,
			ProjectileSpeed,
			false,
			2.f, 0.f,
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

void UTurretRotation::RotateTurret(float DeltaTime)
{
	if (!TurretRef) return;

	const auto TurretRotation = TurretRef->GetComponentRotation().Yaw;
	const auto ResultTurretRotation = FMath::Clamp<float>(ResultRotation.Yaw, TurretMinAngle, TurretMaxAngle);

	//Calculate rotation offset
	float TurretOffset = ResultTurretRotation - TurretRotation;
	TurretOffset = FMath::Wrap(TurretOffset, -180.f, 180.f);

	//get rotation offset for this frame
	const float Offset = FMath::FInterpConstantTo(
		0.f,
		TurretOffset,
		DeltaTime,
		TurretRotationSpeed);

	//apply rotation
	TurretRef->AddRelativeRotation(FRotator(0.f, Offset, 0.f));
}

void UTurretRotation::RotateBarrel(float DeltaTime)
{
	if (!BarrelRef) return;

	const float BarrelRotation = BarrelRef->GetComponentRotation().Pitch;
	const float ResultBarrelRotation = FMath::Clamp<float>(ResultRotation.Pitch, BarrelMinAngle, BarrelMaxAngle);

	//Calculate rotation offset
	float BarrelOffset = ResultBarrelRotation - BarrelRotation;
	BarrelOffset = FMath::Wrap(BarrelOffset, -180.f, 180.f);

	//get rotation offset for this frame
	const float Offset = FMath::FInterpConstantTo(
		0.f,
		BarrelOffset,
		DeltaTime,
		BarrelRotationSpeed);

	//apply rotation
	BarrelRef->AddRelativeRotation(FRotator(Offset, 0.f, 0.f));
}


void UTurretRotation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateResultRotation();
	RotateTurret(DeltaTime);
	RotateBarrel(DeltaTime);
}
