// Fill out your copyright notice in the Description page of Project Settings.


#include "NSFunctionLibrary.h"

FVector UNSFunctionLibrary::GetActorViewPoint_NS(AActor* Pawn, float Length, ECollisionChannel CollisionChannel)
{
	FVector ViewLocation;
	FRotator ViewRotation;
	Pawn->GetActorEyesViewPoint(ViewLocation, ViewRotation);

	FHitResult Hit;
	FVector Start = ViewLocation;
	FVector End = ViewLocation + ViewRotation.Vector()*Length;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Pawn);

	bool bHit = Pawn -> GetWorld()->LineTraceSingleByChannel(Hit, Start, End, CollisionChannel, CollisionParams);

	return bHit ? Hit.ImpactPoint : Hit.TraceEnd;
}