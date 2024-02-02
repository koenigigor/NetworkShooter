// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapWidget.h"

#include "../MinimapController.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "MinimapIconWidget.h"
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

	//init icons
	const FString ObservedLevelName = GetWorld()->GetMapName();
	MinimapController->OnMapObjectAdd.AddUObject(this, &ThisClass::OnMapObjectAdd);
	MinimapController->OnMapObjectUpdate.AddUObject(this, &ThisClass::OnMapObjectUpdate);
	MinimapController->OnMapObjectRemove.AddUObject(this, &ThisClass::OnMapObjectRemove);
	for (const auto& [Name, Icon] : MinimapController->GetMapObjects(ObservedLevelName))
		OnMapObjectAdd(ObservedLevelName, Icon);
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

void UMinimapWidget::OnMapObjectAdd(const FString& LevelName, UMapObjectWrapper* MapObjectContainer)
{
	const FString ObservedLevelName = GetWorld()->GetMapName();
	if (LevelName.Equals(ObservedLevelName))
	{
		const auto MapObject = MapObjectContainer->Get(); //todo get map object for current map type

		// add NEW icon, 
		ensure(!MapObjectsIds.Contains(MapObject->GetUniqueName()));
		AddIcon(MapObject);
	}	
}

void UMinimapWidget::OnMapObjectRemove(const FString& LevelName, UMapObjectWrapper* MapObjectContainer)
{
	const FString ObservedLevelName = GetWorld()->GetMapName();
	if (LevelName.Equals(ObservedLevelName))
	{
		if (MapObjectsIds.Contains(MapObjectContainer->Name))
		{
			RemoveIcon(MapObjectsIds[MapObjectContainer->Name]);
		}
	}
}

void UMinimapWidget::OnMapObjectUpdate(const FString& LevelName, UMapObjectWrapper* MapObjectContainer)
{
	const FString ObservedLevelName = GetWorld()->GetMapName();
	if (LevelName.Equals(ObservedLevelName))
	{
		const auto NewMapObject = MapObjectContainer->Get(); //todo get map object for current map type

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

void UMinimapWidget::AddIcon(UMapObject* MapObject)
{
	//auto MapObject = MapObjectWrapper->Get(); //todo get icon for current map type (global, mini or compass)
	
	if (!IsSatisfiesFilter(MapObject)) return;
	
	const auto IconWidget = MapObject->CreateWidget();
	if (!IconWidget)
	{
		UE_LOG(LogTemp, Display, TEXT("Null icon widget for map object %s"), *MapObject->GetUniqueName())
		return;
	}
	
	const auto ZOrder = MapObject->IsIcon()
		? MapObject->GetLayer() * 10 + 5 + MapObject->WidgetPriority		// Icon Layer + 5 + Priority
		: MapObject->GetLayer() * 10 + MapObject->WidgetPriority;			// Background Layer + Priority;
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

	MapObject->OnLayerChange.AddUObject(this, &ThisClass::OnIconLayerChange);
}

void UMinimapWidget::RemoveIcon(UMapObject* MapObject)
{
	if (MapObjects.Contains(MapObject))
	{
		MapObjects.FindAndRemoveChecked(MapObject)->RemoveFromParent();
		MapObjectsIds.FindAndRemoveChecked(MapObject->GetUniqueName());
		MapObject->OnLayerChange.RemoveAll(this);
	}
}

void UMinimapWidget::OnIconLayerChange(UMapObject* MapObject)
{
	UE_LOG(LogMinimapWidget, Display, TEXT("Icon %s change layer to %d"), *MapObject->GetUniqueName(), MapObject->GetLayer())
	if (!ensure(MapObjects.Contains(MapObject))) return;

	const auto ZOrder = MapObject->GetLayer() * 10 + 9;
	const auto CanvasSlot = StaticCast<UCanvasPanelSlot*>(MapObjects[MapObject]->Slot);
	CanvasSlot->SetZOrder(ZOrder);	
}

void UMinimapWidget::UpdateIconLocations()
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
	//todo if change layer, scale, in out interpolations with sine scale
	
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

	//todo also check layer
}

void UMinimapWidget::SetFilter(FGameplayTagContainer NewFilter)
{
	Filter = NewFilter;
	
	// add remove visible map objects
	const FString ObservedLevelName = GetWorld()->GetMapName();
	const auto MinimapController = UMinimapController::Get(this);
	for (const auto& [Name, MapObjectContainer] : MinimapController->GetMapObjects(ObservedLevelName))
	{
		const auto MapObject = MapObjectContainer->Get();	//todo get map object for current map type
		const auto bVisible = MapObjects.Contains(MapObject);
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

	const FString ObservedLevelName = GetWorld()->GetMapName();
	const auto MinimapController = UMinimapController::Get(this);
	for (const auto& [Name, MapObjectContainer] : MinimapController->GetMapObjects(ObservedLevelName))
	{
		const auto MapObject = MapObjectContainer->Get();	//todo get map object for current map type
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
