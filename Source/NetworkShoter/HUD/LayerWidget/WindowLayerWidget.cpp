// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowLayerWidget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/NSCanvasPanel.h"

DEFINE_LOG_CATEGORY_STATIC(LogWindowLayer, All, All);

UWindowLayerWidget::UWindowLayerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WindowLocations =
	{
		{TAG_WINDOW_CHAT_SETTINGS, FVector2D(0.21f, 0.26f)}
	};
}


int32 UWindowLayerWidget::GetWindowCount() const
{
	return MainCanvas->GetChildrenCount();
}

void UWindowLayerWidget::PushWindow(UWidget* Window)
{
	const auto CanvasSlot = MainCanvas->AddChildToCanvas(Window);
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetPosition(UWidgetLayoutLibrary::GetMousePositionOnViewport(this));
}

void UWindowLayerWidget::PushWindow(FGameplayTag WindowTag, UWidget* Window)
{
	const FVector2D WindowAnchor = WindowLocations.Contains(WindowTag) ? WindowLocations[WindowTag] : FVector2D(0.25f, 0.2f);
	UE_CLOG(!WindowLocations.Contains(WindowTag), LogWindowLayer, Warning, TEXT("Push Window, default location for tag %s, is not set"), *WindowTag.ToString())

	const auto CanvasSlot = MainCanvas->AddChildToCanvas(Window);
	CanvasSlot->SetAnchors(FAnchors(WindowAnchor.X, WindowAnchor.Y));
	CanvasSlot->SetAutoSize(true);

	SpawnedWindows.Add(WindowTag, Window);
	//MakeWeakObjectPtr ^
}

//push window near parent window
void UWindowLayerWidget::PushWindow(UWidget* ParentWindow, UWidget* Window, EWindowSnap ParentSnap, bool bHorizontal)
{
	if (ParentSnap == EWindowSnap::None) ParentSnap = EWindowSnap::TopLeft;
	
	//get window anchor and align parameters por target snap
	auto SnapAnchor = FVector2D::ZeroVector;
	auto WindowAlignment = FVector2D::ZeroVector;
	MainCanvas->CalcSnapAndAlign(ParentWindow, ParentSnap, SnapAnchor, WindowAlignment, bHorizontal);

	//flip snap edge if close to viewport edge
	if (SnapAnchor.X < 0.3f)
	{
		if (ParentSnap == EWindowSnap::TopLeft)
		{
			MainCanvas->CalcSnapAndAlign(ParentWindow, EWindowSnap::TopRight, SnapAnchor, WindowAlignment, bHorizontal);
		}
		else if (ParentSnap == EWindowSnap::BottomLeft)
		{
			MainCanvas->CalcSnapAndAlign(ParentWindow, EWindowSnap::BottomRight, SnapAnchor, WindowAlignment, bHorizontal);
		}
	}
	else if (SnapAnchor.X > 0.7f)
	{
		if (ParentSnap == EWindowSnap::TopRight)
		{
			MainCanvas->CalcSnapAndAlign(ParentWindow, EWindowSnap::TopLeft, SnapAnchor, WindowAlignment, bHorizontal);
		}
		else if (ParentSnap == EWindowSnap::BottomRight)
		{
			MainCanvas->CalcSnapAndAlign(ParentWindow, EWindowSnap::BottomLeft, SnapAnchor, WindowAlignment, bHorizontal);
		}
	}

	const auto CanvasSlot = MainCanvas->AddChildToCanvas(Window);
	CanvasSlot->SetAnchors(FAnchors(SnapAnchor.X, SnapAnchor.Y));
	CanvasSlot->SetAlignment(WindowAlignment);
	CanvasSlot->SetAutoSize(true);
}

void UWindowLayerWidget::RemoveTopWindow()
{
	//find window with higher ZOrder
	int32 HigherZOrder = -1;
	UWidget* TopWindow = nullptr;
	for (const auto& Window : MainCanvas->GetAllChildren())
	{
		if (const auto CanvasSlot = Cast<UCanvasPanelSlot>(Window->Slot))
			if (CanvasSlot->GetZOrder() > HigherZOrder)
			{
				HigherZOrder = CanvasSlot->GetZOrder();
				TopWindow = Window;
			}
	}

	if (TopWindow) TopWindow->RemoveFromParent();
}

void UWindowLayerWidget::RemoveAllWindows()
{
	MainCanvas->ClearChildren();
}

bool UWindowLayerWidget::IsHovered_Slow()
{
	//simple is hovered false on click in context menu
	const auto Mouse = FSlateApplication::Get().GetCursorPos();
	
	for (const auto& Widget : MainCanvas->GetAllChildren())
	{
		const auto WidgetStart = Widget->GetCachedGeometry().LocalToAbsolute(FVector2D::ZeroVector);
		const auto WidgetEnd = Widget->GetCachedGeometry().LocalToAbsolute(Widget->GetCachedGeometry().GetLocalSize());

		const auto bInside = Mouse.ComponentwiseAllGreaterOrEqual(WidgetStart) && Mouse.ComponentwiseAllLessOrEqual(WidgetEnd);
		if (bInside) return true;
	}
	return false;
}


void UWindowLayerWidget::NativeConstruct()
{
	if (MainCanvas)
	{
		MainCanvas->OnSlotAddedDelegate.AddUObject(this, &ThisClass::OnWindowAdded);
		MainCanvas->PreRemoveSlotDelegate.AddUObject(this, &ThisClass::PreRemoveWindow);
		MainCanvas->PostRemoveSlotDelegate.AddUObject(this, &ThisClass::OnWindowRemoved);
	}
	else UE_LOG(LogWindowLayer, Error, TEXT("MainCanvas not found"))

	Super::NativeConstruct();
}

void UWindowLayerWidget::OnWindowAdded(UPanelSlot* PanelSlot)
{
	UE_LOG(LogWindowLayer, Display, TEXT("Add window"))
	OnWindowCountChanged.ExecuteIfBound();
}

void UWindowLayerWidget::OnWindowRemoved(UPanelSlot* PanelSlot)
{
	UE_LOG(LogWindowLayer, Display, TEXT("RemoveWindow"))
	OnWindowCountChanged.ExecuteIfBound();
}

void UWindowLayerWidget::PreRemoveWindow(UPanelSlot* PanelSlot)
{
	//check if is named window, update window anchor location
	for (const auto [Tag, Window] : SpawnedWindows)
	{
		if (Window.IsValid() && Window.Get() == PanelSlot->Content)
		{
			const auto TargetAbsolute = PanelSlot->Content->GetCachedGeometry().LocalToAbsolute(FVector2d::ZeroVector);
			const auto Absolute0 = GetCachedGeometry().LocalToAbsolute(FVector2D::ZeroVector);
			const auto Absolute1 = GetCachedGeometry().LocalToAbsolute(GetCachedGeometry().GetLocalSize());
			const auto NewLocation = (TargetAbsolute - Absolute0) / (Absolute1 - Absolute0);

			WindowLocations.Add(Tag, NewLocation);

			return;
		}
	}
}

