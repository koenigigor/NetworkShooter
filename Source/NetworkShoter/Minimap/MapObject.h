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
	bool IsIcon() const { return bIcon; }
	bool IsScalable() const { return bIcon && bScalable; }
	const FString& GetUniqueName() const;
	FVector GetWorldLocation() const;

	/** Highest value of layer (floor), used as ZOrder in multi layer view */
	int32 GetLayer() const {return Layer; }

	/** Unique layer ID todo */
	int32 GetLayerID() const {return Layer; }
	
	/** Create icon widget, override for specify */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UWidget* CreateWidget();

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLayerChange, UMapObject* MapObject)	
	/** Called on runtime objects when it change own map layer */
    FOnLayerChange OnLayerChange;

	
	
//protected:
	
	/** Is icon or background */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIcon = true;
	
	/** Icon widget has static size or scale with minimap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIcon"))
	bool bScalable = true;

	/** Icon category for filter (shop, npc, chest) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIcon", Categories="Map"))
	FGameplayTag Category = TAG_MAP_DEFAULT;	

	/** eq. Widget z order priority in layer space, [0..3] */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image", meta = (ClampMin=0, ClampMax=3))
	int32 WidgetPriority = 0;	

	mutable FString UniqueName;
	

	/** Default icon world location, auto setup */
	UPROPERTY(EditAnywhere)	//todo how save it without construct event
	FVector InitialLocation;

	UPROPERTY(EditAnywhere) //todo also save
	int32 Layer;
};

/** Simple map object version with static image */
UCLASS()
class UMapObjectSimpleImage : public UMapObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Image", meta=( AllowPrivateAccess="true", DisplayThumbnail="true", DisplayName="Image", AllowedClasses="/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"))
	TObjectPtr<UObject> Image = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	float Size = 128.f;

	virtual UWidget* CreateWidget_Implementation() override;
};



/**	Map objects container
 *	keep map objects with same name (id), replace it by priority
 *	Runtime > External > Baked
 *		- Runtime - MapObject passed from loaded MapObjectComponent
 *		- External - MapObject passed from any external source (save, quest system, etc...)
 *		- Baked - Saved map objects from MapObject component (show unloaded actors, or a different level) */
UCLASS()
class UMapObjectWrapper : public UObject
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

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLayerChange, UMapObjectWrapper* MapObjectWrapper)
	FOnLayerChange OnLayerChange;	

	UMapObjectWrapper* AddBaked(UMapObject* MapObject);
	UMapObjectWrapper* AddExternal(UMapObject* MapObject);
	UMapObjectWrapper* AddRuntime(UMapObject* MapObject);

	void RemoveBaked();
	void RemoveExternal();
	void RemoveRuntime();
	
	FORCEINLINE UMapObject* Get() const
	{
		if (RuntimeObject) return RuntimeObject;
		if (ExternalObject) return ExternalObject;
		if (BakedObject) return BakedObject;
		return nullptr;
	}
	//todo different getters (for global map, for minimap, for compass, by object flags)
	
	
	bool IsValid() const { return BakedObject || ExternalObject || RuntimeObject; }
};
