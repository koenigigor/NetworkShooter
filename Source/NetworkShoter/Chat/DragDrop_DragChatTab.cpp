// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat/DragDrop_DragChatTab.h"

#include "ChatStyleSave.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridSlot.h"
#include "Components/Widget.h"
#include "Widgets/ChatTabButton.h"
#include "Widgets/ChatWindow.h"

DEFINE_LOG_CATEGORY_STATIC(LogDragChatTab, All, All);

void UDragDrop_DragChatTab::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	ProcessDragOverWindow(PointerEvent);
}

void UDragDrop_DragChatTab::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);

	if (PointerEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// cancel operation
		UE_LOG(LogDragChatTab, Display, TEXT("Cancel operation"))
	}
	else if (HoveredWindow)
	{
		DropOnWindow(HoveredWindow);
	}
	else 
	{
		// drop to canvas (not consumed)
		DropOnCanvas(PointerEvent);
	}
}

void UDragDrop_DragChatTab::StartDragOverWindow(UChatWindow* OverChatWindow)
{
	if (HoveredWindow == OverChatWindow) return;
		
	HoveredWindow = OverChatWindow;

	// Hide visual
	if (DefaultDragVisual)
		DefaultDragVisual->SetVisibility(ESlateVisibility::Collapsed);

	// Create fake tab button
	FakeTab = CreateWidget<UChatTabButton>(Tab->GetOwningPlayer(), Tab->GetClass());
	FakeTab->TabID = Tab->TabID;
	FakeTab->LoadStyle();	// load name
	FakeTab->SetRenderOpacity(0.5);

	// Put fake tab in window
	const auto Column = HoveredWindow->GetColumnUnderCursor();
	HoveredWindow->AddTabAtColumn(Column, FakeTab, nullptr);

	// todo disable fade in all chats?
}

void UDragDrop_DragChatTab::ProcessDragOverWindow(const FPointerEvent& PointerEvent)
{
	if (!HoveredWindow) return;

	const auto CurrentColumn = CastChecked<UUniformGridSlot>(FakeTab->Slot)->GetColumn();
	const auto TargetColumn = HoveredWindow->GetColumnUnderCursor();

	if (CurrentColumn != TargetColumn)
	{
		// swap column with 1 step;
		const auto bRightSide = TargetColumn > CurrentColumn;
		const auto NextColumn = bRightSide ? CurrentColumn + 1 : CurrentColumn - 1;
		HoveredWindow->SwapTabs(CurrentColumn, NextColumn);
	}
}

void UDragDrop_DragChatTab::EndDragOverWindow()
{
	if (!HoveredWindow) return;
	
	// Remove fake tab button
	HoveredWindow->RemoveTab(FakeTab);

	// Show visual 
	if (DefaultDragVisual)
		DefaultDragVisual->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	HoveredWindow = nullptr;
}

void UDragDrop_DragChatTab::DropOnCanvas(const FPointerEvent& PointerEvent)
{
	UE_LOG(LogDragChatTab, Log, TEXT("Drop on main canvas"))
	
	// Get window position
	const auto PC = DefaultDragVisual->GetOwningPlayer();
	const auto ScreenGeometry = UWidgetLayoutLibrary::GetPlayerScreenWidgetGeometry(PC);
	const auto WindowAbsolute = DefaultDragVisual->GetCachedGeometry().LocalToAbsolute(FVector2D(0.0));
	const auto WindowPosition = ScreenGeometry.AbsoluteToLocal(WindowAbsolute);
	const auto WindowPositionAbs =  ScreenGeometry.AbsoluteToLocal(WindowAbsolute) / ScreenGeometry.GetLocalSize();

	// Add new window and tab in save
	FChatWindowData NewWindowData;
	NewWindowData.AbsoluteCoordinate = WindowPositionAbs;
	//todo copy settings from current window?
		
	const auto ChatSave = UChatStyleSave::Get(PC);
	const auto WindowID = ChatSave->RegisterNewWindow(NewWindowData);
	ChatSave->RegisterMoveTab(Tab->TabID, WindowID, 0);

	// Spawn window widget
	const auto NewWindow = CreateWidget<UChatWindow>(PC, ChatWindow->GetClass());
	NewWindow->WindowID = WindowID;
	NewWindow->AddTabAtColumn(0, Tab, MessagesBox);
	NewWindow->SelectTab(Tab);

	// Find closest anchor, and position
	const auto bLeftAngle = WindowPositionAbs.X < 0.5;
	const auto bTopAngle = WindowPositionAbs.Y < 0.5;
	const auto Anchor = FVector2D(bLeftAngle ? 0.0 : 1.0, bTopAngle ? 0.0 : 1.0);
	const auto Position = FVector2D(
		bLeftAngle ? WindowPosition.X : WindowPosition.X - ScreenGeometry.GetLocalSize().X,
		bTopAngle ? WindowPosition.Y : WindowPosition.Y - ScreenGeometry.GetLocalSize().Y
		);		

	// Add window on canvas, anchored to closest edge
	const auto CanvasSlot = Canvas->AddChildToCanvas(NewWindow);
	CanvasSlot->SetAnchors(FAnchors(Anchor.X, Anchor.Y));
	CanvasSlot->SetPosition(Position);
	CanvasSlot->SetAutoSize(true);
}

void UDragDrop_DragChatTab::DropOnWindow(UChatWindow* Window)
{
    UE_LOG(LogDragChatTab, Log, TEXT("Drop on chat window"))

	EndDragOverWindow();
	Tab->SetSelection(false);

    const auto Column = Window->GetColumnUnderCursor();
	Window->AddTabAtColumn(Column, Tab, MessagesBox);
	Window->SelectTab(Tab);

	//register move tab
	const auto ChatSave = UChatStyleSave::Get(Window);
	ChatSave->RegisterMoveTab(Tab->TabID, Window->WindowID, Column);
}
