// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UCanvasPanel;

/**
 * Base class for main HUDs widgets,
 * create for future usage
 */
UCLASS(Abstract)
class NETWORKSHOTER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;

	FSimpleDelegate OnAddedToFocusPathDelegate;
	FSimpleDelegate OnRemovedFromFocusPathDelegate;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UCanvasPanel* MainCanvas = nullptr;
};
