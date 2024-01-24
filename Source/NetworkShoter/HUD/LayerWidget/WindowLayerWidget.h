// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NSHudStructs.h"
#include "Blueprint/UserWidget.h"
#include "WindowLayerWidget.generated.h"

class APCNetShooter;
class UNSCanvasPanel;

/**
 * 
 */
UCLASS(Abstract)
class NETWORKSHOTER_API UWindowLayerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UWindowLayerWidget(const FObjectInitializer& ObjectInitializer);

	int32 GetWindowCount() const;

	void PushWindow(FGameplayTag WindowTag, UWidget* Window);
	void PushWindow(UWidget* ParentWindow, UWidget* Window, EWindowSnap ParentSnap, bool bHorizontal = true);

	//remove window wih higher ZOrder
	void RemoveTopWindow();
	void RemoveAllWindows();

	FSimpleDelegate OnWindowCountChanged;

	//PushWindow with tag will be anchored in this coordinates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ForceInlineRow))
	TMap<FGameplayTag, FVector2D> WindowLocations;


protected:
	UPROPERTY(meta = (BindWidget))
	UNSCanvasPanel* MainCanvas = nullptr;

	virtual void NativeConstruct() override;

	void OnWindowAdded(UPanelSlot* PanelSlot);
	void OnWindowRemoved(UPanelSlot* PanelSlot);
	void PreRemoveWindow(UPanelSlot* PanelSlot);

	//Cache spawned named windows, for update WindowLocations when remove window
	TMap<FGameplayTag, TWeakObjectPtr<UWidget>> SpawnedWindows;
};
