// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapLocationSpline.generated.h"

class USplineComponent;
class UBillboardComponent;

/** Editor only actor for setup map locations */
UCLASS()
class NETWORKSHOTER_API AMapLocationSpline : public AActor
{
	GENERATED_BODY()
public:
	AMapLocationSpline();

	UPROPERTY(EditAnywhere)
	FString LocationID;

	UPROPERTY(VisibleAnywhere)
	UBillboardComponent* Billboard = nullptr;

	UPROPERTY(VisibleAnywhere)
	USplineComponent* Spline = nullptr;
};

