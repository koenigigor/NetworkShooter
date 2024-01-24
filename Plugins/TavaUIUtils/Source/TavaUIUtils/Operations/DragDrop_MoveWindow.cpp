// Fill out your copyright notice in the Description page of Project Settings.


#include "Operations/DragDrop_MoveWindow.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Widget.h"


UDragDrop_MoveWindow* UDragDrop_MoveWindow::CreateDragDrop_MoveWindow(UWidget* InWidget, bool InbUpdateAnchors)
{
	const auto Operation = NewObject<UDragDrop_MoveWindow>();
	Operation->Widget = InWidget;
	Operation->bUpdateAnchors = InbUpdateAnchors;
	
	return Operation;
}

UDragDrop_MoveWindow* UDragDrop_MoveWindow::CreateDragDrop_MoveWindow_WithCallback(
	UWidget* InWidget, bool InbUpdateAnchors, FFinishMoveDelegate OnMoveFinish)
{
	const auto Operation = NewObject<UDragDrop_MoveWindow>();
	Operation->Widget = InWidget;
	Operation->bUpdateAnchors = InbUpdateAnchors;
	Operation->Callback = OnMoveFinish;
	
	return Operation;
}

void UDragDrop_MoveWindow::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	if (bFirstTick)
	{
		bFirstTick = false;

		CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
		if (!ensure(CanvasSlot)) return;

		InitialPosition = CanvasSlot->GetPosition();
		InitialMousePositionScr = PointerEvent.GetLastScreenSpacePosition();
	}

	if (!ensure(CanvasSlot)) return;

	//delta mouse from start drag, in local space
	FVector2D LocalDelta;
	const auto ScreenDelta = InitialMousePositionScr - PointerEvent.GetScreenSpacePosition();
	const auto Geometry = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(Widget->GetOwningPlayer());
	USlateBlueprintLibrary::ScreenToWidgetLocal(Widget, Geometry, ScreenDelta, LocalDelta);

	CanvasSlot->SetPosition(InitialPosition - LocalDelta);
}

void UDragDrop_MoveWindow::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);

	//we not consume drop event, so cancel fires anyway, check mouse stl pressed for true cancel
	if (PointerEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		if (!ensure(CanvasSlot)) return;

		CanvasSlot->SetPosition(InitialPosition);
	}
	else
	{
		///true drop
		UpdateAnchors();
		Callback.ExecuteIfBound();
	}
}

void UDragDrop_MoveWindow::UpdateAnchors()
{
	if (!bUpdateAnchors) return;

	//get absolute window position
	const auto ScreenGeometry = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(Widget->GetOwningPlayer());
	const auto WindowPosition = ScreenGeometry.AbsoluteToLocal(Widget->GetCachedGeometry().LocalToAbsolute(FVector2D(0.0)));
	const auto WindowAbsPosition = WindowPosition / ScreenGeometry.GetLocalSize();

	//find closest anchor
	const auto bLeftAngle = WindowAbsPosition.X < 0.5;
	const auto bTopAngle = WindowAbsPosition.Y < 0.5;
	const auto Anchor = FVector2D(bLeftAngle ? 0.0 : 1.0, bTopAngle ? 0.0 : 1.0);

	//calculate anchored position
	const auto Position = FVector2D(
		bLeftAngle ? WindowPosition.X : WindowPosition.X - ScreenGeometry.GetLocalSize().X,
		bTopAngle ? WindowPosition.Y : WindowPosition.Y - ScreenGeometry.GetLocalSize().Y
		);

	//set window position
	CanvasSlot->SetAnchors(FAnchors(Anchor.X, Anchor.Y));
	CanvasSlot->SetPosition(Position);
}
