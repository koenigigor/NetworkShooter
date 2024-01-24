// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestFeed.h"

#include "Components/OverlaySlot.h"

void UQuestFeed::OnSlotAdded(UPanelSlot* InSlot)
{
	Super::OnSlotAdded(InSlot);

	if (const auto OverlaySlot = Cast<UOverlaySlot>(InSlot))
	{
		const auto HorizontalAlignment = (ChildSnapping == EFeedChildSnapping::BottomLeft || ChildSnapping == EFeedChildSnapping::TopLeft) ?
			HAlign_Left : HAlign_Right;
		const auto VerticalAlignment = (ChildSnapping == EFeedChildSnapping::TopRight || ChildSnapping == EFeedChildSnapping::TopLeft) ?
			VAlign_Top : VAlign_Bottom;
		
		OverlaySlot->SetHorizontalAlignment(HorizontalAlignment);
		OverlaySlot->SetVerticalAlignment(VerticalAlignment);
		//OverlaySlot->SetVerticalAlignment(VAlign_Center);
	}
}

#if WITH_EDITOR
void UQuestFeed::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//todo preview value reset on save, need something mark dirty child widget
	
	//no interp version for preview
	FVector2D AccumulatedOffset = FVector2D::ZeroVector;
	for (int32 i = Slots.Num()-1; i >= 0; --i)
	{
		const auto ChildWidget = Slots[i]->Content;
		if (ChildWidget->GetDesiredSize().IsNearlyZero()) continue; 

		const auto TargetTranslation = AccumulatedOffset;
		ChildWidget->SetRenderTranslation(TargetTranslation);

		const auto ItemSize = ChildWidget->GetDesiredSize() + InnerPadding;
		AccumulatedOffset += ItemSize * GetSnappingMultiplier();
		
	}
}
#endif WITH_EDITOR

void UQuestFeed::TickInterpPanels(float DeltaTime)
{
	FVector2D AccumulatedOffset = FVector2D::ZeroVector;
	for (int32 i = Slots.Num()-1; i >= 0; --i)
	{
		const auto ChildWidget = Slots[i]->Content;
		if (ChildWidget->GetDesiredSize().IsNearlyZero()) continue; //first 2 ticks after add DesiredSize is 0, ignore 

		const auto CurrentTranslation = ChildWidget->GetRenderTransform().Translation;
		const auto TargetTranslation = FMath::Vector2DInterpTo(CurrentTranslation, AccumulatedOffset, DeltaTime, InterpSpeed);
		ChildWidget->SetRenderTranslation(TargetTranslation);

		const auto ItemSize = ChildWidget->GetDesiredSize() + InnerPadding;
		AccumulatedOffset += ItemSize * GetSnappingMultiplier();
	}
}

FVector2D UQuestFeed::GetSnappingMultiplier()
{
	TMap<EFeedChildSnapping, FVector2D> MultiplierMap =
	{
		{EFeedChildSnapping::BottomLeft, {1.f, -1.f}},
		{EFeedChildSnapping::BottomRight, {-1.f, -1.f}},
		{EFeedChildSnapping::TopLeft, {1.f, 1.f}},
		{EFeedChildSnapping::TopRight, {-1.f, 1.f}}
	};
	UE_CLOG(!MultiplierMap.Contains(ChildSnapping), LogTemp, Error, TEXT("UQuestFeed::GetSnappingMultiplier invalid ChildSnapping"))

	const auto Value = MultiplierMap.Find(ChildSnapping);
	return Value
		? *Value * (bHorizontalSlide ? FVector2D(1.f, 0.f) : FVector2D(0.f, 1.f))
		: FVector2D::ZeroVector;
}
