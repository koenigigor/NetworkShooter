// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamAttitude.h"
#include "Components/RichTextBlockImageDecorator.h"
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
	static FVector GetActorViewPoint_NS(APawn* Pawn, float Length, ECollisionChannel CollisionChannel);

	/** return unit vector in cone, based on distribution
	 *	larger exponent will cluster points more tightly around the center
	 *  find this distribution formula in Lyra sample */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector GetRandConeNormalDistribution(const FVector& ConeDirection, const float ConeHalfAngle, const float Exponent);

	/** Check FGenericTeamID, if team equal return true */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsSameTeam(AActor* A, AActor* B);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ETeamAttitude::Type GetAttitude(AActor* A, AActor* B);

	UFUNCTION(BlueprintPure)
	static FSlateBrush RichImageRowToSlateBrush(const FRichImageRow& Row)
	{
		return Row.Brush;
	}
};
