// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapWidget.h"

#include "../MinimapController.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Minimap/MapLayerGroup.h"
#include "Minimap/MapObject.h"
#include "Styling/UMGCoreStyle.h"

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

	ObservedLevelName = GetMyMapName(GetWorld());
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

int32 UMinimapWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	DrawDebugLayers(OutDrawElements, LayerId, AllottedGeometry);

	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

FReply UMinimapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseWheel(InGeometry, InMouseEvent);

	ScaleMap(InMouseEvent.GetWheelDelta() * WheelScaleStep);

	return FReply::Handled();
}

#pragma region MapControllerEvents

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

void UMinimapWidget::OnPlayerChangeLayer(const FLayerInfo& NewLayer)
{
	// Apply layer if keep eyes on player
	if (CenterMapState == Player)
	{
		SetObservedLayer(NewLayer);
	}
}

#pragma endregion MapControllerEvents

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

#pragma region StateMachineCenterOfMap

void UMinimapWidget::SetCenterOfMap(FVector2D NewCenter)
{
	CustomCenterOfMap = NewCenter;
	CenterMapState = Custom;

	// End custom mode timer
	if (bAutoFocusPlayer)
	{
		GetWorld()->GetTimerManager().SetTimer(
			BackToFocusPlayerTimer,
			FTimerDelegate::CreateLambda([&]() { CenterMapState = CustomToPlayer; }),
			AutoFocusPlayerTime,
			false);
	}
}

void UMinimapWidget::CenterToPlayer()
{
	GetWorld()->GetTimerManager().ClearTimer(BackToFocusPlayerTimer);
	CenterMapState = Player;

	SetObservedLevel(GetMyMapName(GetWorld()));

	const auto MapController = UMinimapController::Get(this);
	SetObservedLayer(MapController->GetPlayerLayer());
}

void UMinimapWidget::SetAutoFocusPlayer(bool bFocusPlayer)
{
	bAutoFocusPlayer = bFocusPlayer;

	if (bAutoFocusPlayer)
	{
		CenterToPlayer();
	}
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

#pragma endregion StateMachineCenterOfMap

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

FVector UMinimapWidget::MapToWorld(FVector2D MapLocation) const
{
	return (FVector(MapLocation.Y - 1.0, -MapLocation.X, 0.0) + 0.5) * SegmentSize * -1.0;
}

void UMinimapWidget::SetObservedLevel(const FString& LevelName)
{
	if (LevelName.Equals(ObservedLevelName)) return;

	ObservedLevelName = LevelName;
	UE_LOG(LogMinimapWidget, Display, TEXT("SetObservedLevel: %s"), *ObservedLevelName)

	// Destroy all icons
	for (auto [MapObject, Widget] : MapObjects)
	{
		Widget->RemoveFromParent();
	}
	MapObjects.Empty();
	MapObjectsIds.Empty();
	
	RegenerateMap();
	CenterOfMap = {0.0, 0.0};
}

#pragma region IconTagFilter

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

#pragma endregion IconTagFilter

#pragma region LayerFilter

bool UMinimapWidget::IsSatisfiesLayer(UMapObject* MapObject) const
{
	// check layer visibility
	const auto MapController = UMinimapController::Get(this);
	const auto Layers = MapController->GetLayersData(ObservedLevelName);
	if (Layers && !Layers->IsSublayerVisible(MapObject->LayerInfo)) return false;

	// todo also show 'ground' layer icons, if they not overlap layer area
	if (MapObject->IsIcon())
	{
		return MapObject->LayerInfo.IsSameLayer(ObservedLayer);
	}

	// show backgrounds on same layer or ground
	return MapObject->LayerInfo.IsEmpty() || MapObject->LayerInfo.IsSameLayer(ObservedLayer);
}

void UMinimapWidget::SetObservedLayer(const FLayerInfo& NewLayer)
{
	if (ObservedLayer == NewLayer) return;

	ObservedLayer = NewLayer;
	UE_LOG(LogMinimapWidget, Display, TEXT("SetObservedLayer: %s"), *ObservedLayer.ToString())

	RegenerateMap();
}

#pragma endregion LayerFilter

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


void UMinimapWidget::DrawDebugLayers(FSlateWindowElementList& DrawElements, int32 LayerId, const FGeometry& AllottedGeometry) const
{
#if !UE_BUILD_SHIPPING

	auto WorldToWidget = [&](FVector WorldPosition) -> FVector2D
	{
		const auto PointOnMapDelta = WorldToMap(WorldPosition) - CenterOfMap;
		const auto AbsTargetPosition = MarkCanvas->GetCachedGeometry().LocalToAbsolute(
			PointOnMapDelta * MarkCanvas->GetCachedGeometry().GetLocalSize() - MarkCanvas->GetRenderTransform().Translation);
		const auto WidgetStart = AllottedGeometry.LocalToAbsolute(FVector2D::ZeroVector);
		const auto WidgetEnd = AllottedGeometry.LocalToAbsolute(AllottedGeometry.GetLocalSize());

		const auto LocalTargetPositionAbs = (WidgetStart - AbsTargetPosition) / (WidgetStart - WidgetEnd);
		const auto LocalTargetPosition = LocalTargetPositionAbs * AllottedGeometry.GetLocalSize();

		return LocalTargetPosition;
	};

	const auto MapController = UMinimapController::Get(this);
	if (!MapController) return;

	const auto LayersData = MapController->GetLayersData(ObservedLevelName);
	if (!LayersData) return;

	const auto OverlappedGroup = LayersData->GetGroupAtLocation2D(MapToWorld(CenterOfMap));
	const auto WidgetSize = AllottedGeometry.GetLocalSize();

	for (const auto LayerGroup : LayersData->LayerGroups)
	{
		const auto Bounds = LayerGroup->GetBounds();

		TArray<FVector2f> DrawPoints;

		//clamp lines inside widget
		{
			TArray<FVector2D> Points
			{
				WorldToWidget(Bounds.Max),
				WorldToWidget(FVector(Bounds.Max.X, Bounds.Min.Y, 0.0)),
				WorldToWidget(Bounds.Min),
				WorldToWidget(FVector(Bounds.Min.X, Bounds.Max.Y, 0.0))
			};

			// clamp point inside [0, max]
			auto ClampPoint = [](FVector2D Point, FVector2D Max)
			{
				return FVector2D(FMath::Clamp(Point.X, 0.0, Max.X), FMath::Clamp(Point.Y, 0.0, Max.Y));
			};

			auto IsLineIntersectBox = [](FVector2D PointA, FVector2D PointB, FVector2D Max) -> bool
			{
				if (FMath::IsNearlyEqual(PointA.X, PointB.X, 0.1) && PointA.X >= 0.0 && PointA.X <= Max.X)
				{
					return FMath::Min(PointA.Y, PointB.Y) <= Max.Y && FMath::Max(PointA.Y, PointB.Y) >= 0.0;
				}
				if (FMath::IsNearlyEqual(PointA.Y, PointB.Y, 0.1) && PointA.Y >= 0.0 && PointA.Y <= Max.Y)
				{
					return FMath::Min(PointA.X, PointB.X) <= Max.X && FMath::Max(PointA.X, PointB.X) >= 0.0;
				}
				return false;
			};

			bool bAddForward = true;
			for (int32 i = 0; i < Points.Num(); ++i)
			{
				auto LineStartIndex = bAddForward ? i : (-i + Points.Num()) % Points.Num();
				auto LineEndIndex = (LineStartIndex + 1) % Points.Num();
				auto LineStart = Points[LineStartIndex];
				auto LineEnd = Points[LineEndIndex];

				if (IsLineIntersectBox(LineStart, LineEnd, WidgetSize))
				{
					if (DrawPoints.Num() == 0)
					{
						bAddForward
							? DrawPoints.Add(UE::Slate::CastToVector2f(ClampPoint(LineStart, WidgetSize)))
							: DrawPoints.Add(UE::Slate::CastToVector2f(ClampPoint(LineEnd, WidgetSize)));						
					}

					bAddForward
						? DrawPoints.Add(UE::Slate::CastToVector2f(ClampPoint(LineEnd, WidgetSize)))
						: DrawPoints.Insert(UE::Slate::CastToVector2f(ClampPoint(LineStart, WidgetSize)), 0);
				}
				else
				{
					if (!bAddForward) break;
					bAddForward = false;
					i = 0;
				}
			}
		}

		/*
		DrawPoints.Add(UE::Slate::CastToVector2f(WorldToWidget(Bounds.Max)));
		DrawPoints.Add(UE::Slate::CastToVector2f(WorldToWidget(FVector(Bounds.Max.X, Bounds.Min.Y, 0.0))));
		DrawPoints.Add(UE::Slate::CastToVector2f(WorldToWidget(Bounds.Min)));
		DrawPoints.Add(UE::Slate::CastToVector2f(WorldToWidget(FVector(Bounds.Min.X, Bounds.Max.Y, 0.0))));
		DrawPoints.Add(UE::Slate::CastToVector2f(WorldToWidget(Bounds.Max)));
		*/

		if (DrawPoints.IsEmpty()) return;

		FSlateDrawElement::MakeLines(
			DrawElements,
			++LayerId,
			AllottedGeometry.ToPaintGeometry(),
			DrawPoints,
			ESlateDrawEffect::None,
			LayerGroup == OverlappedGroup ? FLinearColor::Green : FLinearColor::Red,
			true,
			2);

		FVector2D TextPosition = WorldToWidget(Bounds.Min);
		bool bTextInsideWidget = TextPosition.X >= 0 && TextPosition.X <= WidgetSize.X && TextPosition.Y >= 0 && TextPosition.Y <= WidgetSize.Y;
		if (bTextInsideWidget)
		{
			FSlateFontInfo FontInfo = FUMGCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText").Font;
			FontInfo.Size += 10;

			FSlateDrawElement::MakeText(
				DrawElements,
				++LayerId,
				AllottedGeometry.ToOffsetPaintGeometry(TextPosition),
				LayerGroup->UniqueName,
				FontInfo,
				ESlateDrawEffect::None,
				LayerGroup == OverlappedGroup ? FLinearColor::Green : FLinearColor::Red);
		}
	}

#endif
}
