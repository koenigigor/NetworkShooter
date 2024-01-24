// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapObjectComponent.generated.h"

class UWidget;
class UMinimapLayerCollider;

/** Primary map object component,
 *	represent background image or icon
 *	override CreateIcon() for create widget.
 *	Visible when any overlapped MinimapLayerCollider is active */
UCLASS(ClassGroup=(Minimap), meta=(BlueprintSpawnableComponent), Blueprintable )
class NETWORKSHOTER_API UMapObjectComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UMapObjectComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Toggle visibility if any layer volume active */
	void UpdateVisibility();	
	void ShowIcon();
	void RemoveIcon();

	bool IsIcon() const {return bIcon;};
	bool IsScalable() const { return bIcon && bScalable; };
	
	/** Get highest owning layer */
	int32 GetLayer() const;

	/** Make trace for manual update OverlappedVolumes list */
	void RefreshLayerVolumes();
	
	/** Create icon widget, override for specify */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UWidget* CreateIcon();

	/** Is icon or background representation */ //todo
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIcon = true;
	
	/** Update icon location on map widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIcon"))
	bool bMovable = true;

	/** Icon widget has static size or scale with minimap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIcon"))
	bool bScalable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Image", meta=( AllowPrivateAccess="true", DisplayThumbnail="true", DisplayName="Image", AllowedClasses="/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"))
	TObjectPtr<UObject> Image = nullptr;

	/** Size of background image */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	float Size = 128.f;

	/** eq. Widget z order priority in layer space, [0..3] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image", meta = (ClampMin=0, ClampMax=3))
	int32 WidgetPriority = 0;

	/** Cache LayerVolume on overlap */
	void AddLayerVolume(UMinimapLayerCollider* Volume);
	void RemoveLayerVolume(UMinimapLayerCollider* Volume);

	DECLARE_MULTICAST_DELEGATE_OneParam(FLayerChangeDelegate, UMapObjectComponent* Icon)
	FLayerChangeDelegate OnLayerChange;

protected:
	TSet<TWeakObjectPtr<UMinimapLayerCollider>> OverlappedVolumes;
};


