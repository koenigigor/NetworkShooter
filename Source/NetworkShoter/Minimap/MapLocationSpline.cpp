// Fill out your copyright notice in the Description page of Project Settings.


#include "MapLocationSpline.h"

#include "Components/BillboardComponent.h"
#include "Components/SplineComponent.h"


AMapLocationSpline::AMapLocationSpline()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bIsEditorOnlyActor = true;

	Billboard = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	SetRootComponent(Billboard);
	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	Spline->SetupAttachment(GetRootComponent());
	Spline->SetClosedLoop(true);
}

