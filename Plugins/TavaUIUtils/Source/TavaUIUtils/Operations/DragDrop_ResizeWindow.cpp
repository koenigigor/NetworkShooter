// Fill out your copyright notice in the Description page of Project Settings.


#include "DragDrop_ResizeWindow.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Misc/TavaUILib.h"

UDragDrop_ResizeWindow* UDragDrop_ResizeWindow::CreateDragDrop_ResizeWindow(UUserWidget* InWidget, USizeBox* InSizeBox, FVector2D InMinSize, FVector2D InMaxSize)
{
	const auto Operation = NewObject<UDragDrop_ResizeWindow>();
	Operation->Widget = InWidget;
	Operation->SizeBox = InSizeBox;
	Operation->MinSize = InMinSize;
	Operation->MaxSize = InMaxSize;
	
	return Operation;
}

UDragDrop_ResizeWindow* UDragDrop_ResizeWindow::CreateDragDrop_ResizeWindow_WithCallback(UUserWidget* InWidget, USizeBox* InSizeBox, FVector2D InMinSize,
	FVector2D InMaxSize, FFinishResizeDelegate OnResizeFinish)
{
	const auto Operation = NewObject<UDragDrop_ResizeWindow>();
	Operation->Widget = InWidget;
	Operation->SizeBox = InSizeBox;
	Operation->MinSize = InMinSize;
	Operation->MaxSize = InMaxSize;
	Operation->Callback = OnResizeFinish;
	
	return Operation;
}

void UDragDrop_ResizeWindow::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);
	if (!ensure(Widget && SizeBox)) return;
	
	if (bFirstTick)
	{
		bFirstTick = false;
		InitialSize = FVector2D(SizeBox->GetWidthOverride(), SizeBox->GetHeightOverride());
		if (const auto CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			InitialPosition = CanvasSlot->GetPosition();
		}
		
		const auto MousePosition = UTavaUILib::GetAbsMousePositionOnWidget(Widget);
		bLeftDragDirection = MousePosition.X < 0.5;
		bUpDragDirection = MousePosition.Y < 0.5;
		InitialMousePositionScr = PointerEvent.GetLastScreenSpacePosition();

		//calculate min max drag positions
		MinPosition.X = InitialPosition.X - (MaxSize.X - InitialSize.X);
		MinPosition.Y = InitialPosition.Y - (MaxSize.Y - InitialSize.Y);
		
		MaxPosition.X = InitialPosition.X + InitialSize.X - MinSize.X;
		MaxPosition.Y = InitialPosition.Y + InitialSize.Y - MinSize.Y;
	}

	//delta size change from start drag position in widget space
	FVector2D LocalDelta;
	const auto ScreenDelta = InitialMousePositionScr - PointerEvent.GetScreenSpacePosition();
	const auto Geometry = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(Widget->GetOwningPlayer());
	USlateBlueprintLibrary::ScreenToWidgetLocal(Widget, Geometry, ScreenDelta, LocalDelta);
	LocalDelta *= FVector2D(bLeftDragDirection ? 1.0 : -1.0, bUpDragDirection ? 1.0 : -1.0);
	
	//new clamped size
	const auto NewWidth = FMath::Clamp(InitialSize.X + LocalDelta.X, MinSize.X, MaxSize.X);
	const auto NewHeight = FMath::Clamp(InitialSize.Y + LocalDelta.Y, MinSize.Y, MaxSize.Y);
	
	//adjust window position on canvas panel
	if (const auto CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
	{		
		const auto CurrentPosition = CanvasSlot->GetPosition();
		
		CanvasSlot->SetPosition(FVector2D(
			bLeftDragDirection ? FMath::Clamp(InitialPosition.X - LocalDelta.X, MinPosition.X, MaxPosition.X) : CurrentPosition.X,
			bUpDragDirection ? FMath::Clamp(InitialPosition.Y - LocalDelta.Y, MinPosition.Y, MaxPosition.Y) : CurrentPosition.Y
		));
	}

	SizeBox->SetWidthOverride(NewWidth);
    SizeBox->SetHeightOverride(NewHeight);
}


void UDragDrop_ResizeWindow::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);

	//we not consume drop event, so cancel fires anyway, check mouse stl pressed for true cancel
	if (PointerEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		///true cancel
		if (!InitialSize.IsNearlyZero())
		{
			SizeBox->SetWidthOverride(InitialSize.X);
			SizeBox->SetHeightOverride(InitialSize.Y);
		}
        
		if (const auto CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot))
		{
			CanvasSlot->SetPosition(InitialPosition);
		}
	}
	else
	{
		///true drop
		Callback.ExecuteIfBound();
	} 
}
