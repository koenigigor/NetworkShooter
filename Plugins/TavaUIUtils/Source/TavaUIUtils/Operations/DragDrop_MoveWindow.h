// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DragDrop_MoveWindow.generated.h"

class UCanvasPanelSlot;


/** Generic operation for move widget (window) inside canvas panel */
UCLASS()
class TAVAUIUTILS_API UDragDrop_MoveWindow : public UDragDropOperation
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_DELEGATE(FFinishMoveDelegate);

	/**	@param bUpdateAnchors Snap widget anchor to closest corner */
	UFUNCTION(BlueprintCallable, Category="Drag and Drop", meta = (DefaultToSelf = "Widget", bUpdateAnchors="true"))
	static UDragDrop_MoveWindow* CreateDragDrop_MoveWindow(
		UWidget* Widget,
		bool bUpdateAnchors);

	/**	@param bUpdateAnchors Snap widget anchor to closest corner */
	UFUNCTION(BlueprintCallable, Category="Drag and Drop", meta = (DefaultToSelf = "Widget", bUpdateAnchors="true"))
	static UDragDrop_MoveWindow* CreateDragDrop_MoveWindow_WithCallback(
		UWidget* Widget,
		bool bUpdateAnchors,
		FFinishMoveDelegate OnMoveFinish);	
	
	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;

protected:
	UPROPERTY(BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true" ))
	UWidget* Widget = nullptr;
	TObjectPtr<UCanvasPanelSlot> CanvasSlot = nullptr;

	/** Snap widget anchor to closest corner */
	UPROPERTY(BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true" ))
	bool bUpdateAnchors = true;

	void UpdateAnchors();
	
	FVector2D InitialPosition;
	FFinishMoveDelegate Callback;

	/** Initial mouse position in screen space */
	FVector2D InitialMousePositionScr;

	/** First Dragged tick, for setup initial variables (like BeginPlay) */
	bool bFirstTick = true;
};
