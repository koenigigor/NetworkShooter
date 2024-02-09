// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapStructures.h"
#include "Components/ActorComponent.h"
#include "MinimapController.generated.h"

class UMapObjectComponent;
class UMapObject;
class UMapObjectContainer;
class UMapLayerGroup;
class UMapLayersData;

/** Map objects on level */
USTRUCT()
struct FMapObjectsCache
{
	GENERATED_BODY()

	/** Key - MapObject ID */
	UPROPERTY()
	TMap<FString, UMapObjectContainer*> Containers;
};

/** Game state component,
 *	Keep map icons and images, in different maps,
 *	Icon can be baked, runtime and external */
UCLASS(ClassGroup=(Minimap), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UMinimapController : public UActorComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, DisplayName = "GetMinimapController", meta = (DefaultToSelf = "WorldContextObject", HidePin = "WorldContextObject"))
	static UMinimapController* Get(const UObject* WorldContextObject);

	UMinimapController();

	void RegisterMapObject(UMapObjectComponent* Icon);
	void UnregisterMapObject(UMapObjectComponent* Icon);

	void AddExternalIcon(UMapObject* MapObject, const FString& MapName, bool bNotify = true);
	void RemoveExternalIcon(UMapObject* MapObject, const FString& MapName, bool bNotify = true);

	/** Return cached map objects for level [UniqueName, Object] */
	const TMap<FString, UMapObjectContainer*>& GetMapObjects(FString LevelName);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FCachedMapObjetDelegate, const FString& LevelName, UMapObjectContainer* MapObject);
	FCachedMapObjetDelegate OnMapObjectAdd;
	FCachedMapObjetDelegate OnMapObjectUpdate; //add/remove new map object type in container
	FCachedMapObjetDelegate OnMapObjectRemove;
	FCachedMapObjetDelegate OnMapObjectChangeLayer;

	DECLARE_MULTICAST_DELEGATE_OneParam(FChangeLayerDelegate, const FLayerInfo& NewLayer);
	FChangeLayerDelegate OnPlayerChangeLayer;
	void SetPlayerLayer(FLayerInfo NewLayer);
	const FLayerInfo& GetPlayerLayer() const { return PlayerLayer; };

	/** Layers data for each level */
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSubclassOf<UMapLayersData>> BakedLayersData;

	/** LevelName - Baked data */
	UPROPERTY()
	TMap<FString, UMapLayersData*> BakedLayers;

	UFUNCTION(BlueprintCallable)
	UMapLayersData* GetLayersData(const FString& MapName);
	
protected:
	FLayerInfo PlayerLayer;

	void AddMapObject_Internal(UMapObject* MapObject, EMapObjectType Type, const FString& MapName, bool bNotify = true);
	void RemoveMapObject_Internal(UMapObject* MapObject, EMapObjectType Type, const FString& MapName, bool bNotify = true);

	TMap<FString, UMapObjectContainer*>& GetMapObjects_Mutable(FString LevelName);

	/** [LevelName - MapObjects on level] */
	UPROPERTY()
	TMap<FString, FMapObjectsCache> MapObjectsCache;
};
