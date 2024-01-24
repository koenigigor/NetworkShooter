// Fill out your copyright notice in the Description page of Project Settings.


#include "NSCanvasPanel.h"

DEFINE_LOG_CATEGORY_STATIC(LogNSCanvas, All, All);

void UNSCanvasPanel::CalcSnapAndAlign(UWidget* ParentWindow, EWindowSnap SnapCorner, FVector2D& OutAnchor, FVector2D& OutAlignment, bool bHorizontal) const
{
	//Transform ParentLocalPoint to absolute point [0,1] (for set anchor child widget) 
	const auto GetAbsPointLambda = [&](FVector2D ParentLocalPoint)-> FVector2D
	{
		const auto TargetAbsolute = ParentWindow->GetCachedGeometry().LocalToAbsolute(ParentLocalPoint);
		const auto Absolute0 = GetCachedGeometry().LocalToAbsolute(FVector2D::ZeroVector);
		const auto Absolute1 = GetCachedGeometry().LocalToAbsolute(GetCachedGeometry().GetLocalSize());

		return (TargetAbsolute - Absolute0) / (Absolute1 - Absolute0);
	};

	const auto ParentSize = ParentWindow->GetCachedGeometry().GetLocalSize();
	switch (SnapCorner)
	{
		case EWindowSnap::TopLeft:
		{
			OutAnchor = GetAbsPointLambda(FVector2D::ZeroVector);
			OutAlignment = bHorizontal ? FVector2D(1.0, 0.0) : FVector2D(0.0, 1.0);

			break;
		}
		case EWindowSnap::TopRight:
		{
			OutAnchor = GetAbsPointLambda(FVector2D(ParentSize.X, 0.0));
			OutAlignment = bHorizontal ? FVector2D(0.0, 0.0) : FVector2D(1.0, 1.0);

			break;
		}
		case EWindowSnap::BottomLeft:
		{
			OutAnchor = GetAbsPointLambda(FVector2D(0.0, ParentSize.Y));
			OutAlignment = bHorizontal ? FVector2D(1.0, 1.0) : FVector2D(0.0, 0.0);

			break;
		}
		case EWindowSnap::BottomRight:
		{
			OutAnchor = GetAbsPointLambda(ParentSize);
			OutAlignment = bHorizontal ? FVector2D(0.0, 1.0) : FVector2D(1.0, 0.0);

			break;
		}
		default:
		{
			UE_LOG(LogNSCanvas, Warning, TEXT("Unknown window snap edge, set to TopLeft"))
			OutAnchor = GetAbsPointLambda(FVector2D::ZeroVector);
			OutAlignment = FVector2D(1.0, 0.0);
		}
	}
}

void UNSCanvasPanel::OnSlotAdded(UPanelSlot* InSlot)
{
	Super::OnSlotAdded(InSlot);

	OnSlotAddedDelegate.Broadcast(InSlot);
}

void UNSCanvasPanel::OnSlotRemoved(UPanelSlot* InSlot)
{
	PreRemoveSlotDelegate.Broadcast(InSlot);

	Super::OnSlotRemoved(InSlot);

	PostRemoveSlotDelegate.Broadcast(InSlot);
}
