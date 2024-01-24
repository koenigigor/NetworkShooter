// Fill out your copyright notice in the Description page of Project Settings.


#include "FeedChildSnappingCustomization.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/QuestFeed.h"

//https://unrealcommunity.wiki/customizing-details-amp-property-type-panel-tutorial-00deskro
//HorizontalAlignmentCustomization.cpp
//CanvasSlotCustomization.cpp

#define LOCTEXT_NAMESPACE "FeedSnapping"

TSharedRef<IPropertyTypeCustomization> FFeedChildSnappingCustomization::MakeInstance()
{
	return MakeShareable(new FFeedChildSnappingCustomization());
}

void FFeedChildSnappingCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	HeaderRow
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MaxDesiredWidth(0)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		[
			SNew(SSegmentedControl<EFeedChildSnapping>)
			.Value(this, &FFeedChildSnappingCustomization::GetCurrentAlignment, PropertyHandle)
			.OnValueChanged(this, &FFeedChildSnappingCustomization::OnCurrentAlignmentChanged, PropertyHandle)
			+SSegmentedControl<EFeedChildSnapping>::Slot(EFeedChildSnapping::TopLeft)
				.Icon(FAppStyle::GetBrush("HorizontalAlignment_Left"))
				.ToolTip(LOCTEXT("TopLeft", "TopLeft"))
			+ SSegmentedControl<EFeedChildSnapping>::Slot(EFeedChildSnapping::TopRight)
				.Icon(FAppStyle::GetBrush("HorizontalAlignment_Center"))
				.ToolTip(LOCTEXT("TopRight", "TopRight"))
			+ SSegmentedControl<EFeedChildSnapping>::Slot(EFeedChildSnapping::BottomLeft)
				.Icon(FAppStyle::GetBrush("HorizontalAlignment_Right"))
				.ToolTip(LOCTEXT("BottomLeft", "BottomLeft"))
			+ SSegmentedControl<EFeedChildSnapping>::Slot(EFeedChildSnapping::BottomRight)
				.Icon(FAppStyle::GetBrush("HorizontalAlignment_Fill"))
				.ToolTip(LOCTEXT("BottomRight", "BottomRight"))
		]
		/*
		+SVerticalBox::Slot()
		[
			SNew(SSegmentedControl<EFeedChildSnapping>)
			.Value(this, &FFeedChildSnappingCustomization::GetCurrentAlignment, PropertyHandle)
			.OnValueChanged(this, &FFeedChildSnappingCustomization::OnCurrentAlignmentChanged, PropertyHandle)
			+SSegmentedControl<EFeedChildSnapping>::Slot(EFeedChildSnapping::TopLeft)
				.Icon(FAppStyle::GetBrush("HorizontalAlignment_Left"))
				.ToolTip(LOCTEXT("TopLeft", "TopLeft"))
			+ SSegmentedControl<EFeedChildSnapping>::Slot(EFeedChildSnapping::TopRight)
				.Icon(FAppStyle::GetBrush("HorizontalAlignment_Center"))
				.ToolTip(LOCTEXT("TopRight", "TopRight"))
			+ SSegmentedControl<EFeedChildSnapping>::Slot(EFeedChildSnapping::BottomLeft)
				.Icon(FAppStyle::GetBrush("HorizontalAlignment_Right"))
				.ToolTip(LOCTEXT("BottomLeft", "BottomLeft"))
			+ SSegmentedControl<EFeedChildSnapping>::Slot(EFeedChildSnapping::BottomRight)
				.Icon(FAppStyle::GetBrush("HorizontalAlignment_Fill"))
				.ToolTip(LOCTEXT("BottomRight", "BottomRight"))
		]
		*/	
	];
}

void FFeedChildSnappingCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder,
	IPropertyTypeCustomizationUtils& CustomizationUtils)
{

}

EFeedChildSnapping FFeedChildSnappingCustomization::GetCurrentAlignment(
	TSharedRef<IPropertyHandle, ESPMode::ThreadSafe> PropertyHandle) const
{
	uint8 Value;
	if (PropertyHandle->GetValue(Value) == FPropertyAccess::Result::Success)
	{
		return (EFeedChildSnapping)Value;
	}

	return EFeedChildSnapping::BottomRight;
}

void FFeedChildSnappingCustomization::OnCurrentAlignmentChanged(EFeedChildSnapping NewFeedChildSnapping,
	TSharedRef<IPropertyHandle, ESPMode::ThreadSafe> PropertyHandle)
{
	PropertyHandle->SetValue((uint8)NewFeedChildSnapping);
}

#undef LOCTEXT_NAMESPACE
