// Fill out your copyright notice in the Description page of Project Settings.


#include "Minimap/MinimapController.h"

#include "MapObject.h"
#include "MapObjectComponent.h"
#include "GameFramework/GameStateBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogMinimapController, All, All);


UMinimapController* UMinimapController::Get(UObject* WorldContextObject)
{
	if (WorldContextObject && WorldContextObject->GetWorld() && WorldContextObject->GetWorld()->GetGameState())
	{
		const auto MapController = WorldContextObject->GetWorld()->GetGameState()->FindComponentByClass<UMinimapController>();
		UE_CLOG(!MapController, LogMinimapController, Warning, TEXT("Minimap controller must be game state component, ensure it pug into GameState"));
		return MapController;
	}

	return nullptr;
}

UMinimapController::UMinimapController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMinimapController::RegisterMapObject(UMapObjectComponent* Icon)
{
	RuntimeMapComponents.Add(Icon);

	// update cached map objects, call delegates
	const auto LevelName = GetWorld()->GetMapName();
	AddRuntime_Internal(Icon->MapObject, LevelName);
}

void UMinimapController::UnregisterMapObject(UMapObjectComponent* Icon)
{	
	RuntimeMapComponents.Remove(Icon);

	// update cached map objects, call delegates
	const auto LevelName = GetWorld()->GetMapName();
	RemoveRuntime_Internal(Icon->MapObject, LevelName);
}


TMap<FString, UMapObjectWrapper*>& UMinimapController::GetMapObjects_Mutable(FString LevelName)
{
	if (MapObjectsCache.Contains(LevelName))
	{
		return MapObjectsCache[LevelName];
	}

	UE_LOG(LogMinimapController, Log, TEXT("Load map objects for level %s"), *LevelName)

	auto& MapObjectsRow = MapObjectsCache.Add(LevelName);
	
	// Add baked objects in cache	//todo
	//GetMutableDefaults...
	TArray<UMapObject*> BakedObjects{};
	for (const auto& BakedObject : BakedObjects)
	{
		AddBaked_Internal(BakedObject, LevelName, false);
	}

	//todo external objects
		//todo how about level (add property in Object)
		//todo how about pointer to different level door
	
	return MapObjectsCache[LevelName];
}

const TMap<FString, UMapObjectWrapper*>&  UMinimapController::GetMapObjects(FString LevelName)
{
	return GetMapObjects_Mutable(LevelName);
}


void UMinimapController::AddBaked_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify)
{
	const auto& ObjectName = MapObject->GetUniqueName();
	auto& MapObjectsRow = GetMapObjects_Mutable(MapName);
	
	if (MapObjectsRow.Contains(ObjectName))
	{
		const auto MapObjectWrapper = MapObjectsRow[ObjectName]->AddBaked(MapObject);
		if (bNotify)
			OnMapObjectUpdate.Broadcast(MapName, MapObjectWrapper);
	}
	else
	{
		const auto MapObjectWrapper = MapObjectsRow.Add(ObjectName, NewObject<UMapObjectWrapper>()->AddBaked(MapObject));
		if (bNotify)
			OnMapObjectAdd.Broadcast(MapName, MapObjectWrapper);
	}
}

void UMinimapController::RemoveBaked_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify)
{
	const auto& ObjectName = MapObject->GetUniqueName();
	auto& MapObjectsRow = GetMapObjects_Mutable(MapName);
	
	const auto MapObjectWrapper = MapObjectsRow[ObjectName];
	check(MapObjectWrapper);
	
	MapObjectWrapper->RemoveBaked();
	if (!MapObjectWrapper->IsValid())
	{
		MapObjectsRow.Remove(ObjectName);
		if (bNotify)
			OnMapObjectRemove.Broadcast(MapName, MapObjectWrapper);
	}
	else
	{
		if (bNotify)
			OnMapObjectUpdate.Broadcast(MapName, MapObjectWrapper);
	}
}

void UMinimapController::AddRuntime_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify)
{
	const auto& ObjectName = MapObject->GetUniqueName();
	auto& MapObjectsRow = GetMapObjects_Mutable(MapName);
	
	if (MapObjectsRow.Contains(ObjectName))
	{
		const auto MapObjectWrapper = MapObjectsRow[ObjectName]->AddRuntime(MapObject);
		if (bNotify)
			OnMapObjectUpdate.Broadcast(MapName, MapObjectWrapper);
	}
	else
	{
		const auto MapObjectWrapper = MapObjectsRow.Add(ObjectName, NewObject<UMapObjectWrapper>()->AddRuntime(MapObject));
		if (bNotify)
			OnMapObjectAdd.Broadcast(MapName, MapObjectWrapper);
	}	
}

void UMinimapController::RemoveRuntime_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify)
{
	const auto& ObjectName = MapObject->GetUniqueName();
	auto& MapObjectsRow = GetMapObjects_Mutable(MapName);

	if (!ensure(MapObjectsRow.Contains(ObjectName))) return;
	
	const auto MapObjectWrapper = MapObjectsRow[ObjectName];
	check(MapObjectWrapper);
	
	MapObjectWrapper->RemoveRuntime();
	if (!MapObjectWrapper->IsValid())
	{
		MapObjectsRow.Remove(ObjectName);
		if (bNotify)
			OnMapObjectRemove.Broadcast(MapName, MapObjectWrapper);
	}
	else
	{
		if (bNotify)
			OnMapObjectUpdate.Broadcast(MapName, MapObjectWrapper);
	}
}

void UMinimapController::AddExternal_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify)
{
	const auto& ObjectName = MapObject->GetUniqueName();
	auto& MapObjectsRow = GetMapObjects_Mutable(MapName);
	
	if (MapObjectsRow.Contains(ObjectName))
	{
		const auto MapObjectWrapper = MapObjectsRow[ObjectName]->AddExternal(MapObject);
		if (bNotify)
			OnMapObjectUpdate.Broadcast(MapName, MapObjectWrapper);
	}
	else
	{
		const auto MapObjectWrapper = MapObjectsRow.Add(ObjectName, NewObject<UMapObjectWrapper>()->AddExternal(MapObject));
		if (bNotify)
			OnMapObjectAdd.Broadcast(MapName, MapObjectWrapper);
	}
}

void UMinimapController::RemoveExternal_Internal(UMapObject* MapObject, const FString& MapName, bool bNotify)
{
	const auto& ObjectName = MapObject->GetUniqueName();
	auto& MapObjectsRow = GetMapObjects_Mutable(MapName);
	
	const auto MapObjectWrapper = MapObjectsRow[ObjectName];
	check(MapObjectWrapper);
	
	MapObjectWrapper->RemoveExternal();
	if (!MapObjectWrapper->IsValid())
	{
		MapObjectsRow.Remove(ObjectName);
		if (bNotify)
			OnMapObjectRemove.Broadcast(MapName, MapObjectWrapper);
	}
	else
	{
		if (bNotify)
			OnMapObjectUpdate.Broadcast(MapName, MapObjectWrapper);
	}
}

