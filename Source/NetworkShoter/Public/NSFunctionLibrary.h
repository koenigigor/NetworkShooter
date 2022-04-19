// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NSFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UNSFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Simple line trace from pawn view to get point where he actual view */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector GetActorViewPoint_NS(AActor* Pawn, float Length, ECollisionChannel CollisionChannel);

	/** return unit vector in cone, based on distribution
	 *	larger exponent will cluster points more tightly around the center
	 *  find this distribution formula in Lyra sample */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector GetRandConeNormalDistribution(const FVector& ConeDirection, const float ConeHalfAngle, const float Exponent);
};
