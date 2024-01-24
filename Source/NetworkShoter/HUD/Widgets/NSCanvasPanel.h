// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSHudStructs.h"
#include "Components/CanvasPanel.h"
#include "NSCanvasPanel.generated.h"


UCLASS()
class NETWORKSHOTER_API UNSCanvasPanel : public UCanvasPanel
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotChangeDelegate, UPanelSlot* InSlot);
	FOnSlotChangeDelegate OnSlotAddedDelegate;
	FOnSlotChangeDelegate PreRemoveSlotDelegate;
	FOnSlotChangeDelegate PostRemoveSlotDelegate;

	/** Calculate anchor and alignment for add item to canvas around specified widget */
	void CalcSnapAndAlign(UWidget* ParentWindow, EWindowSnap SnapCorner, FVector2D& OutAnchor, FVector2D& OutAlignment, bool bHorizontal = true) const;

protected:
	virtual void OnSlotAdded(UPanelSlot* InSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* InSlot) override;
};
