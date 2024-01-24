// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

class UMapObjectComponent;
class UMinimapIconWidget;
class UCanvasPanel;
class UMinimapController;
class UImage;
class AMinimapBackground;

/** Minimap widget contain background and icons
 *	ZOrder calculation:
 *		Background: Level * 10 + SubLevel[0..3] (eq. on 2 floor is 20-23 ZOrders)
 *		Icon: Level * 10 + 5 + SubLevel[0..3] (eq. on 2 floor is 25-28 ZOrders)
 *		Last 24 and 29 reserved */
UCLASS(Abstract)
class NETWORKSHOTER_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION() void AddIcon(UMapObjectComponent* Icon);
	UFUNCTION() void RemoveIcon(UMapObjectComponent* Icon);

	void OnIconLayerChange(UMapObjectComponent* Icon);

	void UpdateCenterOfMap(float DeltaTime);
    void MoveMap();
	void UpdateIconLocations();
	void ScaleMap(float Delta);

	/** New center equals Abs position on Mark canvas */
	UFUNCTION(BlueprintCallable)
	void SetCenterOfMap(FVector2D NewCenter);
	UFUNCTION(BlueprintPure)
	FVector2D GetCenterOfMap() const { return CenterOfMap; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRotateMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scale")
	float WheelScaleStep = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scale")
	float MinScale = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scale")
	float MaxScale = 5.00f;

	//UV Scale [0,1]
	float SegmentSize = 10000.f;

protected:	
	/** Get pivot on Mark canvas based on world location */
	FVector2D WorldToMap(FVector WorldLocation) const;
	
	UPROPERTY() TMap<UMapObjectComponent*, UWidget*> IconObjects;

	
	/** Map (widget) center in map space */
	FVector2D CenterOfMap;

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
	FTimerHandle UseCustomCenterOfMapTimer;
	FVector2D CustomCenterOfMap;

protected:
	/** Player centred panel, mark canvas moves along it */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* RootCanvas = nullptr;

	/** Panel with icons and backgrounds */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCanvasPanel* MarkCanvas = nullptr;
};

