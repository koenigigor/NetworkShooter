// Fill out your copyright notice in the Description page of Project Settings.


#include "NSFunctionLibrary.h"

#include "GenericTeamAgentInterface.h"

FVector UNSFunctionLibrary::GetActorViewPoint_NS(APawn* Pawn, float Length, ECollisionChannel CollisionChannel)
{
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNSFunctionLibrary::GetActorViewPoint_NS not pawn"))
		return FVector::ZeroVector;
	};
	
	FVector ViewLocation = Pawn->GetPawnViewLocation();
	FRotator ViewRotation = Pawn->GetBaseAimRotation();
	//Pawn->GetActorEyesViewPoint(ViewLocation, ViewRotation);

	FHitResult Hit;
	FVector Start = ViewLocation;
	FVector End = ViewLocation + ViewRotation.Vector()*Length;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Pawn);

	bool bHit = Pawn -> GetWorld()->LineTraceSingleByChannel(Hit, Start, End, CollisionChannel, CollisionParams);

	return bHit ? Hit.ImpactPoint : Hit.TraceEnd;
}

FVector UNSFunctionLibrary::GetRandConeNormalDistribution(const FVector& ConeDirection, const float ConeHalfAngle,
	const float Exponent)
{
	if (ConeHalfAngle <= 0.f)
	{
		return ConeDirection.GetSafeNormal();
	}

	// consider the cone a concatenation of two rotations. one "away" from the center line, and another "around" the circle
	// apply the exponent to the away-from-center rotation. a larger exponent will cluster points more tightly around the center
	const float FromCenter = FMath::Pow(FMath::FRand(), Exponent);
	const float AngleFromCenter = FromCenter * ConeHalfAngle;
	const float AngleAround = FMath::FRand() * 360.0f;

	FRotator Rot = ConeDirection.Rotation();
	FQuat DirQuat(Rot);
	FQuat FromCenterQuat(FRotator(0.0f, AngleFromCenter, 0.0f));
	FQuat AroundQuat(FRotator(0.0f, 0.0, AngleAround));
	FQuat FinalDirectionQuat = DirQuat * AroundQuat * FromCenterQuat;
	FinalDirectionQuat.Normalize();

	return FinalDirectionQuat.RotateVector(FVector::ForwardVector);
}

bool UNSFunctionLibrary::IsSameTeam(AActor* A, AActor* B)
{
	return FGenericTeamId::GetTeamIdentifier(A) == FGenericTeamId::GetTeamIdentifier(B);
}

ETeamAttitude::Type UNSFunctionLibrary::GetAttitude(AActor* A, AActor* B)
{
	return FGenericTeamId::GetAttitude(A, B);
}
