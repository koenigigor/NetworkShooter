// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "UObject/Object.h"
#include "MapBounds.generated.h"


/** Baked data about world location border,
 *	Only 2D, based on spine */
UCLASS(DefaultToInstanced, EditInlineNew)
class NETWORKSHOTER_API ULocationBound : public UObject
{
	GENERATED_BODY()
public:
	void InitSpline(const TArray<FVector2D>& Points);
	
	/** Position component from spline */
	UPROPERTY(EditAnywhere)
	FInterpCurveVector2D CurveVector;

	UFUNCTION(BlueprintPure)
	bool IsPointInside2D_Simple(FVector WorldPosition) const;

	UFUNCTION(BlueprintPure)
	bool IsPointInside2D_Complex(FVector WorldPosition) const;


	UFUNCTION(BlueprintCallable)
	void DrawDebug(UObject* WorldContext, float InputKeyStep = 1.f);
};


/** Describes single map location (region) */
UCLASS(DefaultToInstanced, EditInlineNew)
class NETWORKSHOTER_API UMapLocation : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString LocationID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	ULocationBound* Location = nullptr;

	// visibility condition
};


/** List of all baked locations per level */
UCLASS(Abstract)
class NETWORKSHOTER_API UBakedMapLocations : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UMapLocation*> MapLocations;
};
