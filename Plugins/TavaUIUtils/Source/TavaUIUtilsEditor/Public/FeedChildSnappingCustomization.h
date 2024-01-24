// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "Widgets\QuestFeed.h"

class FFeedChildSnappingCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	EFeedChildSnapping GetCurrentAlignment(TSharedRef<IPropertyHandle, ESPMode::ThreadSafe> PropertyHandle) const;
	void OnCurrentAlignmentChanged(EFeedChildSnapping NewFeedChildSnapping, TSharedRef<IPropertyHandle, ESPMode::ThreadSafe> PropertyHandle);
};
