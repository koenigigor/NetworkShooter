// Fill out your copyright notice in the Description page of Project Settings.


#include "MapBounds.h"

#include "Components/SplineComponent.h"

void ULocationBound::InitSpline(const TArray<FVector2D>& Points)
{
	const int32 NumPoints = Points.Num();
	CurveVector.Points.Reset(NumPoints);

	float InputKey = 0.0f;
	for (const auto& Point : Points)
	{
		CurveVector.Points.Emplace(InputKey, Point, FVector2D::ZeroVector, FVector2D::ZeroVector, CIM_CurveAuto);

		InputKey += 1.0f;
	}

	// Ensure splines' looping status matches with that of the spline component
	const float LastKey = CurveVector.Points.Num() > 0 ? CurveVector.Points.Last().InVal : 0.0f;
	const float LoopKey = LastKey + 1.0f;
	CurveVector.SetLoopKey(LoopKey);

	// Automatically set the tangents on any CurveAuto keys
	CurveVector.AutoSetTangents(0.0f, false);
}

bool ULocationBound::IsPointInside2D_Simple(FVector WorldPosition) const
{
	FVector2D Min, Max;
	CurveVector.CalcBounds(Min, Max);

	const FBox2D Box(Min, Max);
	return Box.IsInside(FVector2D(WorldPosition));
}

bool ULocationBound::IsPointInside2D_Complex(FVector WorldPosition) const
{
	const FVector2D Position(WorldPosition);

	FVector2D Min, Max;
	CurveVector.CalcBounds(Min, Max);
	const FVector2D Center = FMath::Lerp(Min, Max, 0.5f);

	float Distance;
	const float InputKey = CurveVector.InaccurateFindNearest(FVector2D(WorldPosition), Distance);
	const FVector2D ClosestSplinePosition = CurveVector.Eval(InputKey, FVector2D::ZeroVector);

	return FVector2D::DistSquared(Center, Position) < FVector2D::DistSquared(Center, ClosestSplinePosition);
}


void ULocationBound::DrawDebug(UObject* WorldContext, float InputKeyStep)
{
	const auto World = WorldContext ? WorldContext->GetWorld() : nullptr;
	if (!World) return;

	if (InputKeyStep <= 0.01) InputKeyStep = 0.1;

	const auto NumPoints = CurveVector.Points.Num();
	const auto NumSteps = FMath::Floor(NumPoints / InputKeyStep);

	auto PreviousPoint = FVector(CurveVector.Eval(0.f, FVector2D::ZeroVector), 0);
	for (int32 i = 1; i <= NumSteps; ++i)
	{
		const auto InputKey = InputKeyStep * i;
		const auto Point = FVector(CurveVector.Eval(InputKey, FVector2D::ZeroVector), 0);
		DrawDebugLine(World, PreviousPoint, Point, FColor::Green, false, 10.f, 0, 10);

		PreviousPoint = Point;
	}
	DrawDebugLine(World, PreviousPoint, FVector(CurveVector.Eval(0.f, FVector2D::ZeroVector), 0), FColor::Green, false, 10.f, 0, 10);
}

