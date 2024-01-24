// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DragDrop_ResizeWindow.generated.h"

class USizeBox;


/** A generic operation for resize window
 *	(maybe there is a more suitable place, but now i think dd operation is good choice) */
UCLASS()
class TAVAUIUTILS_API UDragDrop_ResizeWindow : public UDragDropOperation
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_DELEGATE(FFinishResizeDelegate);
	
	/** Short blueprint creation instead create drag drop operation */
	UFUNCTION(BlueprintCallable, Category="Drag and Drop", meta=(DefaultToSelf = "Widget", MinSize="(X=50.0, Y=50.0)", MaxSize="(X=1000.0, Y=1000.0)"))
	static UDragDrop_ResizeWindow* CreateDragDrop_ResizeWindow(
		UUserWidget* Widget,
		USizeBox* SizeBox,
		FVector2D MinSize,
		FVector2D MaxSize);

	UFUNCTION(BlueprintCallable, Category="Drag and Drop", meta=(DefaultToSelf = "Widget", MinSize="(X=50.0, Y=50.0)", MaxSize="(X=1000.0, Y=1000.0)"))
	static UDragDrop_ResizeWindow* CreateDragDrop_ResizeWindow_WithCallback(
		UUserWidget* Widget,
		USizeBox* SizeBox,
		FVector2D MinSize,
		FVector2D MaxSize,
		FFinishResizeDelegate OnResizeFinish);	

	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;

	UPROPERTY(BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true" ))
	UUserWidget* Widget = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="Drag and Drop", meta=( ExposeOnSpawn="true" ))
	USizeBox* SizeBox = nullptr;

	FVector2D MinSize = FVector2D(50.0, 50.0);
	FVector2D MaxSize = FVector2D(1000.0, 1000.0);

protected:
	FVector2D InitialSize = FVector2D::ZeroVector;
	FVector2D InitialPosition = FVector2D::ZeroVector;
	bool bUpDragDirection = false;
	bool bLeftDragDirection = false;
	FFinishResizeDelegate Callback;

	/** Initial drag mouse position in screen space */
	FVector2D InitialMousePositionScr;

	/** Clamp min and max window position, for window not slide if drag more then min size */
	FVector2D MinPosition;
	FVector2D MaxPosition;

	/** First Dragged tick, for setup initial variables (like BeginPlay) */
	bool bFirstTick = true;
};
