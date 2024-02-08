// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapStructures.h"
#include "Components/ActorComponent.h"
#include "MinimapController.generated.h"

class UMapObjectComponent;
class UMapObject;
class UMapObjectWrapper;
class UMapLayerStack;

/** Game state component,
 *	Keep map icons and images, in different maps,
 *	Icon can be baked, runtime and external */
UCLASS( ClassGroup=(Minimap), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UMinimapController : public UActorComponent
{
	GENERATED_BODY()
public:	
	UFUNCTION(BlueprintPure, DisplayName = "GetMinimapController", meta = (DefaultToSelf = "WorldContextObject", HidePin = "WorldContextObject"))
	static UMinimapController* Get(UObject* WorldContextObject);
	
	UMinimapController();

	void RegisterMapObject(UMapObjectComponent* Icon);
	void UnregisterMapObject(UMapObjectComponent* Icon);

	/** Return cached map objects for level [UniqueName, Object] */
	const TMap<FString, UMapObjectWrapper*>& GetMapObjects(FString LevelName);
	TMap<FString, UMapObjectWrapper*>& GetMapObjects_Mutable(FString LevelName);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FCachedMapObjetDelegate, const FString& LevelName, UMapObjectWrapper* MapObject);
	FCachedMapObjetDelegate OnMapObjectAdd;
	FCachedMapObjetDelegate OnMapObjectUpdate;	//add/remove new map object type in container
	FCachedMapObjetDelegate OnMapObjectRemove;
	FCachedMapObjetDelegate OnMapObjectChangeLayer;

	DECLARE_MULTICAST_DELEGATE_OneParam(FChangeLayerDelegate, const FLayerInfo& NewLayer);
	FChangeLayerDelegate OnPlayerChangeLayer;
	void SetPlayerLayer(FLayerInfo NewLayer);
	const FLayerInfo& GetPlayerLayer() const { return PlayerLayer; };
	FLayerInfo PlayerLayer;

	/** All loaded runtime map objects (registered and unregistered on begin play) */
	UPROPERTY(BlueprintReadOnly)
	TArray<UMapObjectComponent*> RuntimeMapComponents;

	/** External map objects (ex. from quest, saved points etc) */
	UPROPERTY()
	TArray<UMapObject*> ExternalMapObjects;

	void AddBaked_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify = true);
	void RemoveBaked_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify = true);
	void AddRuntime_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify = true);
	void RemoveRuntime_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify = true);
	void AddExternal_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify = true);
	void RemoveExternal_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify = true);
	
	/** Cache map objects with override order Baked > External > Runtime if has same unique name
	 *	[LevelName - TMap<ObjectName, Combined objects>]*/
	TMap<FString, TMap<FString, UMapObjectWrapper*>> MapObjectsCache; //todo strong pointer
	//TMap<FString, UMapLayerStack*> LayerStackCache;
};
