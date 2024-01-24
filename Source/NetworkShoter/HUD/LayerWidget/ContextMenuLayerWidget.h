// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ContextMenuLayerWidget.generated.h"

class UNSCanvasPanel;

/** Layer for add context menus */
UCLASS(Abstract)
class NETWORKSHOTER_API UContextMenuLayerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UNSCanvasPanel* Canvas = nullptr;
};
