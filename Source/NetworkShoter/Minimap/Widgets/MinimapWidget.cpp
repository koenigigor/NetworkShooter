// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapWidget.h"

#include "../MinimapController.h"
#include "../MapObjectComponent.h"
#include "../MinimapBackground.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "MinimapIconWidget.h"
#include "Components/Image.h"

DEFINE_LOG_CATEGORY_STATIC(LogMinimapWidget, All, All);

void UMinimapWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	const auto CanvasSlot = StaticCast<UCanvasPanelSlot*>(MarkCanvas->Slot);
	CanvasSlot->SetSize(FVector2D(SegmentSize));
}

void UMinimapWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const auto MinimapController = UMinimapController::Get(this);

	//init icons
	MinimapController->OnIconAdd.AddDynamic(this, &ThisClass::AddIcon);
	MinimapController->OnIconRemove.AddDynamic(this, &ThisClass::RemoveIcon);
	for (const auto& Icon : MinimapController->VisibleMapObjects)
		AddIcon(Icon);
	
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateCenterOfMap(InDeltaTime);
	MoveMap();
	UpdateIconLocations();
}

FReply UMinimapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	
	ScaleMap(InMouseEvent.GetWheelDelta() * WheelScaleStep);
	
	return FReply::Handled();
}

void UMinimapWidget::AddIcon(UMapObjectComponent* Icon)
{
	if (!IsSatisfiesFilter(Icon)) return;
	
	const auto IconWidget = Icon->CreateIcon();
	if (!IconWidget)
	{
		UE_LOG(LogTemp, Display, TEXT("Null icon widget in %s"), *Icon->GetOwner()->GetName())
		return;
	}
	
	const auto ZOrder = Icon->IsIcon()
		? Icon->GetLayer() * 10 + 5 + Icon->WidgetPriority		// Icon Layer + 5 + Priority
		: Icon->GetLayer() * 10 + Icon->WidgetPriority;			// Background Layer + Priority;
	const auto UV = WorldToMap(Icon->GetOwner()->GetActorLocation());

	const auto CanvasSlot = MarkCanvas->AddChildToCanvas(IconWidget);
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CanvasSlot->SetAnchors(FAnchors(UV.X, UV.Y));
	CanvasSlot->SetZOrder(ZOrder);

	if (Icon->IsIcon() && Icon->bScalable)
	{
		const auto ReverseScale = FVector2d(1.f) / RootCanvas->GetRenderTransform().Scale;
		IconWidget->SetRenderScale(ReverseScale);
	}
	
	IconObjects.Add(Icon, IconWidget);

	Icon->OnLayerChange.AddUObject(this, &ThisClass::OnIconLayerChange);
}

void UMinimapWidget::RemoveIcon(UMapObjectComponent* Icon)
{
	if (IconObjects.Contains(Icon))
	{
		IconObjects.FindAndRemoveChecked(Icon)->RemoveFromParent();
		Icon->OnLayerChange.RemoveAll(this);
	}
}

void UMinimapWidget::OnIconLayerChange(UMapObjectComponent* Icon)
{
	UE_LOG(LogMinimapWidget, Display, TEXT("Icon %s change layer to %d"), *Icon->GetName(), Icon->GetLayer())
	if (!ensure(IconObjects.Contains(Icon))) return;

	const auto ZOrder = Icon->GetLayer() * 10 + 9;
	const auto CanvasSlot = StaticCast<UCanvasPanelSlot*>(IconObjects[Icon]->Slot);
	CanvasSlot->SetZOrder(ZOrder);
}

void UMinimapWidget::UpdateIconLocations()
{
	for (auto [Icon, Widget] : IconObjects)
	{
		if (Icon->bMovable)
		{
			const auto UV = WorldToMap(Icon->GetOwner()->GetActorLocation());
			const auto CanvasSlot = StaticCast<UCanvasPanelSlot*>(Widget->Slot);
			CanvasSlot->SetAnchors(FAnchors(UV.X, UV.Y));
		}
	} 
}

void UMinimapWidget::ScaleMap(float Delta)
{
	const auto NewScale = (RootCanvas->GetRenderTransform().Scale + Delta).ClampAxes(MinScale, MaxScale);
	RootCanvas->SetRenderScale(NewScale);
	
	//scale icons
	const auto ReverseScale = FVector2d(1.0) / NewScale;
	for (const auto& [Icon, Widget] : IconObjects)
	{
		if (Icon->IsScalable())
		{
			Widget->SetRenderScale(ReverseScale);
		}
	}
}

void UMinimapWidget::SetCenterOfMap(FVector2D NewCenter)
{
	//todo target layer, scale, in out interpolations with sine scale
	
	CustomCenterOfMap = NewCenter;
	CenterMapState = Custom;
	
	// set timer for end custom mode
	constexpr float ResetMapTime = 5.f;
	GetWorld()->GetTimerManager().SetTimer(
		UseCustomCenterOfMapTimer,
		FTimerDelegate::CreateLambda([&](){ CenterMapState = CustomToPlayer; }),
		ResetMapTime,
		false);
}

void UMinimapWidget::UpdateCenterOfMap(float DeltaTime)
{
	// also switch layers, use world location?
	
	if (CenterMapState == Player)
	{
		CenterOfMap = WorldToMap(GetOwningPlayer()->GetFocalLocation());
		return;
	}

	if (CenterMapState == Custom)
	{
		CenterOfMap = CustomCenterOfMap;
		return;
	}

	if (CenterMapState == PlayerToCustom)
	{
		CenterOfMap = FMath::Vector2DInterpTo(CenterOfMap, CustomCenterOfMap, DeltaTime, 5.f);
		if (CenterOfMap.Equals(CustomCenterOfMap, 0.01))
		{
			CenterMapState = Custom;
		}
		return;		
	}

	if (CenterMapState == CustomToPlayer)
	{
		const auto PlayerLocation = WorldToMap(GetOwningPlayer()->GetFocalLocation());
		CenterOfMap = FMath::Vector2DInterpTo(CenterOfMap, PlayerLocation, DeltaTime, 5.f);
		if (CenterOfMap.Equals(PlayerLocation, 0.01))
		{
			CenterMapState = Player;
		}
		return;
	}
}

void UMinimapWidget::MoveMap()
{
	//alignment disable widget with 0.175 offset from any corner
	//const auto CanvasSlot = StaticCast<UCanvasPanelSlot*>(MarkCanvas->Slot);
	//CanvasSlot->SetAlignment(CenterOfMap);
	MarkCanvas->SetRenderTranslation(CenterOfMap * MarkCanvas->GetCachedGeometry().GetLocalSize() * -1.0);

	if (bRotateMap)
	{
		const float Angle = -GetOwningPlayer()->GetControlRotation().Yaw;
		
		MarkCanvas->SetRenderTransformPivot(CenterOfMap);
		MarkCanvas->SetRenderTransformAngle(Angle);
	}
}

void UMinimapWidget::CenterToPlayer()
{
	GetWorld()->GetTimerManager().ClearTimer(UseCustomCenterOfMapTimer);
	CenterMapState = Player;
}

FVector2D UMinimapWidget::WorldToMap(FVector WorldLocation) const
{
	//center map is 0 0 0
	const auto UV = (FVector2d(WorldLocation) / SegmentSize) - 0.5;
	return FVector2D(1.0 + UV.Y, -UV.X);
}

bool UMinimapWidget::IsSatisfiesFilter(UMapObjectComponent* Icon) const
{
	if (!Icon) return false;
	if (!Icon->IsIcon()) return true; // background

#if WITH_EDITOR
	if (!Icon->FilterCategory.IsValid())
	{
		UE_LOG(LogMinimapWidget, Warning, TEXT("Empty category for map icon %s, item will be ignored in package"), *Icon->GetOwner()->GetActorNameOrLabel())
		return true;
	}
#endif

	return Filter.HasTag(Icon->FilterCategory);
}

void UMinimapWidget::SetFilter(FGameplayTagContainer NewFilter)
{
	Filter = NewFilter;
	
	// add remove visible map objects
	const auto MinimapController = UMinimapController::Get(this);
	for (const auto& MapObject : MinimapController->VisibleMapObjects)
	{
		const auto bVisible = IconObjects.Contains(MapObject);
		const auto bMustVisible = IsSatisfiesFilter(MapObject);
		
		if (!bVisible && bMustVisible)
		{
			AddIcon(MapObject);
		}
		else if (bVisible && !bMustVisible)
		{
			RemoveIcon(MapObject);
		}
	} 
}

void UMinimapWidget::AddFilter(FGameplayTag Tag)
{
	Filter.AddTag(Tag);

	const auto MinimapController = UMinimapController::Get(this);
	for (const auto& MapObject : MinimapController->VisibleMapObjects)
	{
		const auto bVisible = IconObjects.Contains(MapObject);
		const auto bMustVisible = IsSatisfiesFilter(MapObject);
		
		if (!bVisible && bMustVisible)
		{
			AddIcon(MapObject);
		}
	} 
}

void UMinimapWidget::RemoveFilter(FGameplayTag Tag)
{
	Filter.RemoveTag(Tag);

	for (auto It = IconObjects.CreateIterator(); It; ++It)
	{
		if (!IsSatisfiesFilter(It.Key()))
			RemoveIcon(It.Key());
	}
}
