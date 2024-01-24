// Fill out your copyright notice in the Description page of Project Settings.


#include "TavaUILib.h"

#include "Components/RichTextBlock.h"

void UTavaUILib::SetFontSize(URichTextBlock* RichText, int32 NewSize)
{
	if (!RichText) return;
	
	auto CurrentStyle = RichText->GetCurrentDefaultTextStyle();
	CurrentStyle.Font.Size = NewSize;
	RichText->SetDefaultTextStyle(CurrentStyle);
}

FVector2D UTavaUILib::GetAbsMousePositionOnWidget(UWidget* Widget)
{
	if (!Widget || !FSlateApplication::IsInitialized()) return FVector2D::ZeroVector;

	const auto Mouse = FSlateApplication::Get().GetCursorPos();
	const auto WidgetStart = Widget->GetCachedGeometry().LocalToAbsolute(FVector2D::ZeroVector);
	const auto WidgetEnd = Widget->GetCachedGeometry().LocalToAbsolute(Widget->GetCachedGeometry().GetLocalSize());

	return (WidgetStart - Mouse) / (WidgetStart - WidgetEnd);
}

FVector2D UTavaUILib::GetAbsMousePositionOnWidget(UWidget* Widget, FMargin Padding)
{
	if (!Widget || !FSlateApplication::IsInitialized()) return FVector2D::ZeroVector;

	const auto Mouse = FSlateApplication::Get().GetCursorPos();
	const auto WidgetStart = Widget->GetCachedGeometry().LocalToAbsolute(FVector2D::ZeroVector - FVector2D(Padding.Left, Padding.Top));
	const auto WidgetEnd = Widget->GetCachedGeometry().LocalToAbsolute(Widget->GetCachedGeometry().GetLocalSize() + FVector2D(Padding.Right, Padding.Bottom));

	return (WidgetStart - Mouse) / (WidgetStart - WidgetEnd);	
}

FVector2D UTavaUILib::GetMousePositionOnWidget(UWidget* Widget)
{
	return Widget ? Widget->GetCachedGeometry().GetLocalPositionAtCoordinates(GetAbsMousePositionOnWidget(Widget)) : FVector2D::ZeroVector;
}

