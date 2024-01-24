// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TavaUILib.generated.h"

class URichTextBlock;
class UWidget;

/** Usefully blueprint library */
UCLASS()
class TAVAUIUTILS_API UTavaUILib : public UObject
{
	GENERATED_BODY()
public:
	//by some reason can't set default font size via Blueprint
	UFUNCTION(BlueprintCallable, Category="Appearance")
	static void SetFontSize(URichTextBlock* RichText, int32 NewSize);

	/**	Get absolute (0, 1) mouse position on widget 
	 *	@return Absolute (0, 1) mouse position on widget */
	UFUNCTION(BlueprintPure, Category="Widget", meta=(Keywords="GetMousePositionOnWidget"))
	static FVector2D GetAbsMousePositionOnWidget(UWidget* Widget);
	
	static FVector2D GetAbsMousePositionOnWidget(UWidget* Widget, FMargin Padding);
	
	/**	Get mouse position on widget in local space.
	 *	@return Mouse position on widget (in local space) */
	UFUNCTION(BlueprintPure, Category="Widget")
	static FVector2D GetMousePositionOnWidget(UWidget* Widget);
};
