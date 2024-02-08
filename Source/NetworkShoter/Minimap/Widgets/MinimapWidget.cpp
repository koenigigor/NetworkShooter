// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapWidget.h"

#include "../MinimapController.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Minimap/MapObject.h"

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

	ObservedLevelName = GetWorld()->GetMapName();
	ObservedLayer = MinimapController->GetPlayerLayer();

	MinimapController->OnMapObjectAdd.AddUObject(this, &ThisClass::OnMapObjectAdd);
	MinimapController->OnMapObjectUpdate.AddUObject(this, &ThisClass::OnMapObjectUpdate);
	MinimapController->OnMapObjectRemove.AddUObject(this, &ThisClass::OnMapObjectRemove);
	MinimapController->OnMapObjectChangeLayer.AddUObject(this, &ThisClass::OnMapObjectChangeLayer);
	MinimapController->OnPlayerChangeLayer.AddUObject(this, &ThisClass::OnPlayerChangeLayer);
	for (const auto& [Name, Icon] : MinimapController->GetMapObjects(ObservedLevelName))
		OnMapObjectAdd(ObservedLevelName, Icon);
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateCenterOfMap(InDeltaTime);
	MoveMap();
	MoveIcons();
}

FReply UMinimapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseWheel(InGeometry, InMouseEvent);

	ScaleMap(InMouseEvent.GetWheelDelta() * WheelScaleStep);

	return FReply::Handled();
}

void UMinimapWidget::OnMapObjectAdd(const FString& LevelName, UMapObjectContainer* MapObjectContainer)
{
	if (LevelName.Equals(ObservedLevelName))
	{
		const auto MapObject = MapObjectContainer->Get();

		// add NEW icon, 
		ensure(!MapObjectsIds.Contains(MapObject->GetUniqueName()));
		AddIcon(MapObject);
	}
}

void UMinimapWidget::OnMapObjectRemove(const FString& LevelName, UMapObjectContainer* MapObjectContainer)
{
	if (LevelName.Equals(ObservedLevelName))
	{
		if (MapObjectsIds.Contains(MapObjectContainer->Name))
		{
			RemoveIcon(MapObjectsIds[MapObjectContainer->Name]);
		}
	}
}

void UMinimapWidget::OnMapObjectUpdate(const FString& LevelName, UMapObjectContainer* MapObjectContainer)
{
	if (LevelName.Equals(ObservedLevelName))
	{
		const auto NewMapObject = MapObjectContainer->Get();

		if (MapObjectsIds.Contains(MapObjectContainer->Name))
		{
			const auto CurrentMapObject = MapObjectsIds[MapObjectContainer->Name];

			// was added low priority map object, ignore
			if (CurrentMapObject == NewMapObject) return;

			//remove current, add new
			RemoveIcon(CurrentMapObject);
			AddIcon(NewMapObject);
		}
		else
		{
			AddIcon(NewMapObject);
		}
	}
}

void UMinimapWidget::OnMapObjectChangeLayer(const FString& LevelName, UMapObjectContainer* MapObjectContainer)
{
	if (ObservedLevelName.Equals(LevelName))
	{
		if (MapObjectsIds.Contains(MapObjectContainer->Name))
		{
			// update spawned icon
			const auto MapObject = MapObjectsIds[MapObjectContainer->Name];
			if (IsSatisfiesLayer(MapObject))
			{
				const auto ZOrder = MapObject->GetFloorAbs() * 10 + 5 + MapObject->WidgetPriority;
				const auto CanvasSlot = StaticCast<UCanvasPanelSlot*>(MapObjects[MapObject]->Slot);
				CanvasSlot->SetZOrder(ZOrder);
			}
			else
			{
				RemoveIcon(MapObject);
			}
		}
		else
		{
			// try add new icon
			AddIcon(MapObjectContainer->Get());
		}
	}
}

void UMinimapWidget::AddIcon(UMapObject* MapObject)
{
	if (!(IsSatisfiesFilter(MapObject) && IsSatisfiesLayer(MapObject))) return;

	const auto IconWidget = MapObject->CreateWidget(GetOwningPlayer());
	if (!IconWidget)
	{
		UE_LOG(LogTemp, Display, TEXT("Null icon widget for map object %s"), *MapObject->GetUniqueName())
		return;
	}

	const auto ZOrder = MapObject->IsIcon()
		                    ? MapObject->GetFloorAbs() * 10 + 5 + MapObject->WidgetPriority // Icon Layer + 5 + Priority
		                    : MapObject->GetFloorAbs() * 10 + MapObject->WidgetPriority;    // Background Layer + Priority;
	const auto UV = WorldToMap(MapObject->GetWorldLocation());

	const auto CanvasSlot = MarkCanvas->AddChildToCanvas(IconWidget);
	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CanvasSlot->SetAnchors(FAnchors(UV.X, UV.Y));
	CanvasSlot->SetZOrder(ZOrder);

	if (MapObject->IsScalable())
	{
		const auto ReverseScale = FVector2d(1.f) / RootCanvas->GetRenderTransform().Scale;
		IconWidget->SetRenderScale(ReverseScale);
	}

	MapObjects.Add(MapObject, IconWidget);
	MapObjectsIds.Add(MapObject->GetUniqueName(), MapObject);
}

void UMinimapWidget::RemoveIcon(UMapObject* MapObject)
{
	if (MapObjects.Contains(MapObject))
	{
		MapObjects.FindAndRemoveChecked(MapObject)->RemoveFromParent();
		MapObjectsIds.FindAndRemoveChecked(MapObject->GetUniqueName());
	}
}

void UMinimapWidget::MoveIcons()
{
	//todo different array for movable objects, for reduce costs
	for (auto [MapObject, Widget] : MapObjects)
	{
		//if (MapObject->Get()->IsRuntimeObject())
		{
			const auto UV = WorldToMap(MapObject->GetWorldLocation());
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
	for (const auto& [MapObject, Widget] : MapObjects)
	{
		if (MapObject->IsScalable())
		{
			Widget->SetRenderScale(ReverseScale);
		}
	}
}

void UMinimapWidget::SetCenterOfMap(FVector2D NewCenter)
{
	CustomCenterOfMap = NewCenter;
	CenterMapState = Custom;

	// set timer for end custom mode
	constexpr float ResetMapTime = 5.f;
	GetWorld()->GetTimerManager().SetTimer(
		UseCustomCenterOfMapTimer,
		FTimerDelegate::CreateLambda([&]() { CenterMapState = CustomToPlayer; }),
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

void UMinimapWidget::CenterToPlayer()
{
	GetWorld()->GetTimerManager().ClearTimer(UseCustomCenterOfMapTimer);
	CenterMapState = Player;
}

void UMinimapWidget::MoveMap()
{
	MarkCanvas->SetRenderTranslation(CenterOfMap * MarkCanvas->GetCachedGeometry().GetLocalSize() * -1.0);

	if (bRotateMap)
	{
		const float Angle = -GetOwningPlayer()->GetControlRotation().Yaw;

		MarkCanvas->SetRenderTransformPivot(CenterOfMap);
		MarkCanvas->SetRenderTransformAngle(Angle);
	}
}

FVector2D UMinimapWidget::WorldToMap(FVector WorldLocation) const
{
	//center map is 0 0 0
	const auto UV = (FVector2d(WorldLocation) / SegmentSize) - 0.5;
	return FVector2D(1.0 + UV.Y, -UV.X);
}

bool UMinimapWidget::IsSatisfiesFilter(UMapObject* MapObject) const
{
	// check tags filter
	if (!MapObject) return false;
	if (!MapObject->IsIcon()) return true; // background

#if WITH_EDITOR
	if (!MapObject->Category.IsValid())
	{
		UE_LOG(LogMinimapWidget, Warning, TEXT("Invalid category for icon %s"), *MapObject->GetUniqueName())
		return true;
	}
#endif

	return Filter.HasTag(MapObject->Category);
}

void UMinimapWidget::SetFilter(FGameplayTagContainer NewFilter)
{
	Filter = NewFilter;

	// add remove visible map objects
	RegenerateMap();
}

void UMinimapWidget::AddFilter(FGameplayTag Tag)
{
	Filter.AddTag(Tag);

	const auto MinimapController = UMinimapController::Get(this);
	for (const auto& [Name, MapObjectContainer] : MinimapController->GetMapObjects(ObservedLevelName))
	{
		const auto MapObject = MapObjectContainer->Get();
		const auto bVisible = MapObjects.Contains(MapObject);
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

	for (auto It = MapObjects.CreateIterator(); It; ++It)
	{
		if (!IsSatisfiesFilter(It.Key()))
			RemoveIcon(It.Key());
	}
}

bool UMinimapWidget::IsSatisfiesLayer(UMapObject* MapObject) const
{
	// todo get map controller -> is layer visible

	// todo show icon on same layer, or ground who not overlap layer
	if (MapObject->IsIcon())
	{
		return MapObject->LayerInfo.IsSameLayer(ObservedLayer);
	}

	// show backgrounds on same layer or ground
	return MapObject->LayerInfo.IsEmpty() || MapObject->LayerInfo.IsSameLayer(ObservedLayer);
}

void UMinimapWidget::SetObservedLayer(const FLayerInfo& NewLayer)
{
	ObservedLayer = NewLayer;
	UE_LOG(LogMinimapWidget, Display, TEXT("SetObservedLayer: %s"), *ObservedLayer.ToString())

	RegenerateMap();
}

void UMinimapWidget::OnPlayerChangeLayer(const FLayerInfo& NewLayer)
{
	//todo if keep eyes on player
	SetObservedLayer(NewLayer);
}

void UMinimapWidget::RegenerateMap()
{
	const auto MinimapController = UMinimapController::Get(this);
	for (const auto& [Name, MapObjectContainer] : MinimapController->GetMapObjects(ObservedLevelName))
	{
		const auto MapObject = MapObjectContainer->Get();
		const auto bVisible = MapObjects.Contains(MapObject);
		const auto bMustVisible = IsSatisfiesFilter(MapObject) && IsSatisfiesLayer(MapObject);

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
