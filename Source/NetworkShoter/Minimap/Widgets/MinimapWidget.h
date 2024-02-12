// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Minimap/MapStructures.h"
#include "MinimapWidget.generated.h"

class UMapObject;
class UMapObjectContainer;
class UCanvasPanel;
class UImage;

/** Minimap widget contain background and icons
 *	ZOrder calculation:
 *		Background: Level * 10 + SubLevel[0..3] (eq. on 2 floor is 20-23 ZOrders)
 *		Icon: Level * 10 + 5 + SubLevel[0..3] (eq. on 2 floor is 25-28 ZOrders)
 *		Last 24 and 29 reserved */
UCLASS(Abstract)
class NETWORKSHOTER_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void OnMapObjectAdd(const FString& LevelName, UMapObjectContainer* MapObjectContainer);
	void OnMapObjectUpdate(const FString& LevelName, UMapObjectContainer* MapObjectContainer);
	void OnMapObjectRemove(const FString& LevelName, UMapObjectContainer* MapObjectContainer);
	void OnMapObjectChangeLayer(const FString& LevelName, UMapObjectContainer* MapObjectContainer);
	void OnPlayerChangeLayer(const FLayerInfo& NewLayer);

	void AddIcon(UMapObject* MapObject);
	void RemoveIcon(UMapObject* MapObject);

	void UpdateCenterOfMap(float DeltaTime);
	void MoveMap();
	void MoveIcons();
	void ScaleMap(float Delta);

public:
	UFUNCTION(BlueprintCallable)
	void SetCenterOfMap(FVector2D NewCenter);
	UFUNCTION(BlueprintPure)
	FVector2D GetCenterOfMap() const { return CenterOfMap; };

	/** Reset custom center of map, return to player */
	UFUNCTION(BlueprintCallable)
	void CenterToPlayer();


	UFUNCTION(BlueprintCallable)
	void SetFilter(FGameplayTagContainer NewFilter);
	UFUNCTION(BlueprintCallable)
	void AddFilter(FGameplayTag Tag);
	UFUNCTION(BlueprintCallable)
	void RemoveFilter(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	void SetAutoFocusPlayer(bool bFocusPlayer);
	UFUNCTION(BlueprintCallable)
	void SetObservedLevel(const FString& LevelName);
	void SetObservedLayer(const FLayerInfo& NewLayer);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bRotateMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scale")
	float WheelScaleStep = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scale")
	float MinScale = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scale")
	float MaxScale = 5.00f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Filter", meta = (Categories="Map"))
	FGameplayTagContainer Filter;


protected:
	/** Get pivot on Mark canvas based on world location */
	FVector2D WorldToMap(FVector WorldLocation) const;
	FVector MapToWorld(FVector2D MapLocation) const;

	bool IsSatisfiesFilter(UMapObject* MapObject) const;
	bool IsSatisfiesLayer(UMapObject* MapObject) const;

	/** Iterate all icons on level, enable/disable it by filter and layer */
	void RegenerateMap();

	void DrawDebugLayers(FSlateWindowElementList& DrawElements, int32 LayerId, const FGeometry& AllottedGeometry) const;
	
protected:
	/** World to map UV Scale [0,1] */
	float SegmentSize = 10000.f;

	FLayerInfo ObservedLayer;

	FString ObservedLevelName;

	/** Map (widget) center in map space */
	FVector2D CenterOfMap;

	/** Center of map follow for player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bAutoFocusPlayer = true;

	/** Timer return to player after manual move map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AutoFocusPlayerTime = 5.f;

	/** Center of map state machine:
	 *	Player ->
	 *	Player to Custom interpolation ->
	 *	Custom (skip interpolation if manual drag) ->
	 *	Custom to Player (after delay) -> Player */
	enum ECenterMapState
	{
		Player,
		PlayerToCustom,
		Custom,
		CustomToPlayer
	};

	ECenterMapState CenterMapState;
	FTimerHandle BackToFocusPlayerTimer;
	FVector2D CustomCenterOfMap;


	UPROPERTY()
	TMap<UMapObject*, UWidget*> MapObjects;
	UPROPERTY()
	TMap<FString, UMapObject*> MapObjectsIds;

	/// Widgets	
protected:
	/** Player centred panel, mark canvas moves along it */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* RootCanvas = nullptr;

	/** Panel with icons and backgrounds */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* MarkCanvas = nullptr;
};
