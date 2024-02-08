// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapStructures.h"
#include "UObject/Object.h"
#include "MapObject.generated.h"

class UWidget;


/** Base class for any map object (icon, clickable icon, background) */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable)
class UMapObject : public UObject
{
	GENERATED_BODY()
public:
	virtual UWorld* GetWorld() const override;

	bool IsIcon() const { return bIcon; }
	bool IsScalable() const { return bIcon && bScalable; }
	const FString& GetUniqueName() const;
	FVector GetWorldLocation() const;

	FString GetLayerGroup() const { return LayerInfo.LayerGroup; }
	FString GetSublayer() const { return LayerInfo.Sublayer; }
	int32 GetFloor() const { return LayerInfo.Floor; }
	int32 GetFloorAbs() const { return FMath::Abs(LayerInfo.Floor); }

	/** Create icon widget, override for specify */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UWidget* CreateWidget(APlayerController* PC);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLayerChange, UMapObject* MapObject)
	/** Called on runtime objects when it change own map layer */
	FOnLayerChange OnLayerChange;

	UPROPERTY(VisibleAnywhere, Category = "Layer")
	FLayerInfo LayerInfo;

	//protected:

	/** Is icon or background */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Icon")
	bool bIcon = true;

	/** Icon widget has static size or scale with minimap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Icon", meta = (EditCondition = "bIcon"))
	bool bScalable = true;

	/** Icon category for filter (shop, npc, chest) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Icon", meta = (EditCondition = "bIcon", Categories="Map"))
	FGameplayTag Category = TAG_MAP_DEFAULT;

	/** eq. Widget z order priority in layer space, [0..3] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Icon", meta = (ClampMin=0, ClampMax=3))
	int32 WidgetPriority = 0;

	mutable FString UniqueName;

	/** Default icon world location, auto setup */
	UPROPERTY(EditAnywhere) //todo save initial value
	FVector InitialLocation;
};

/** Simple map object version with static image */
UCLASS()
class UMapObjectSimpleImage : public UMapObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Image",
		meta=( DisplayThumbnail="true", DisplayName="Image", AllowedClasses=
			"/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses =
			"/Script/MediaAssets.MediaTexture"))
	TObjectPtr<UObject> Image = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	float Size = 128.f;

	virtual UWidget* CreateWidget_Implementation(APlayerController* PC) override;
};


/**	Container for same map objects (by ID), but different priority.
 *	Priority: Runtime > External > Baked
 *		- Runtime - MapObject passed from loaded MapObjectComponent
 *		- External - MapObject passed from any external source (save, quest system, etc...)
 *		- Baked - Saved map objects from MapObject component (show unloaded actors, or a different level) */
UCLASS()
class UMapObjectContainer : public UObject
{
	GENERATED_BODY()
public:	
	UPROPERTY()
	UMapObject* BakedObject = nullptr;
	UPROPERTY()
	UMapObject* ExternalObject = nullptr;
	UPROPERTY()
	UMapObject* RuntimeObject = nullptr;

	FString Name;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLayerChange, UMapObjectContainer* MapObjectWrapper)
	FOnLayerChange OnLayerChange;

	UMapObjectContainer* Add(UMapObject* MapObject, EMapObjectType Type);
	void Remove(EMapObjectType Type);
	
	UMapObjectContainer* AddBaked(UMapObject* MapObject);
	UMapObjectContainer* AddExternal(UMapObject* MapObject);
	UMapObjectContainer* AddRuntime(UMapObject* MapObject);

	void RemoveBaked();
	void RemoveExternal();
	void RemoveRuntime();

	UMapObject* Get() const;

	bool IsValid() const { return BakedObject || ExternalObject || RuntimeObject; }
};
