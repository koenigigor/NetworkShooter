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
	const auto LevelName = GetWorld()->GetMapName();
	AddMapObject_Internal(Icon->MapObject, Runtime, LevelName);
}

void UMinimapController::UnregisterMapObject(UMapObjectComponent* Icon)
{
	const auto LevelName = GetWorld()->GetMapName();
	RemoveMapObject_Internal(Icon->MapObject, Runtime, LevelName);
}


TMap<FString, UMapObjectContainer*>& UMinimapController::GetMapObjects_Mutable(FString LevelName)
{
	if (MapObjectsCache.Contains(LevelName))
	{
		return MapObjectsCache[LevelName].Containers;
	}

	UE_LOG(LogMinimapController, Log, TEXT("Load map objects for level %s"), *LevelName)

	MapObjectsCache.Add(LevelName);

	// todo load baked (and external?) objects for different level
	/*
	//GetMutableDefaults...
	TArray<UMapObject*> BakedObjects{};
	for (const auto& BakedObject : BakedObjects)
	{
		AddMapObject_Internal(BakedObject, EMapObjectType::Baked, LevelName, false);
	}
	*/

	return MapObjectsCache[LevelName].Containers;
}

const TMap<FString, UMapObjectContainer*>& UMinimapController::GetMapObjects(FString LevelName)
{
	return GetMapObjects_Mutable(LevelName);
}


void UMinimapController::SetPlayerLayer(FLayerInfo NewLayer)
{
	PlayerLayer = NewLayer;

	//todo ignore if same floor and new sublayer not change visibility?
	OnPlayerChangeLayer.Broadcast(PlayerLayer);
}


void UMinimapController::AddExternalIcon(UMapObject* MapObject, const FString& MapName, bool bNotify)
{
	AddMapObject_Internal(MapObject, External, MapName, bNotify);
}

void UMinimapController::RemoveExternalIcon(UMapObject* MapObject, const FString& MapName, bool bNotify)
{
	RemoveMapObject_Internal(MapObject, External, MapName, bNotify);
}

void UMinimapController::AddMapObject_Internal(UMapObject* MapObject, EMapObjectType Type, const FString& MapName, bool bNotify)
{
	const auto& ObjectName = MapObject->GetUniqueName();
	auto& MapObjectsRow = GetMapObjects_Mutable(MapName);

	if (MapObjectsRow.Contains(ObjectName))
	{
		const auto MapObjectWrapper = MapObjectsRow[ObjectName]->Add(MapObject, Type);
		if (bNotify)
			OnMapObjectUpdate.Broadcast(MapName, MapObjectWrapper);
	}
	else
	{
		const auto MapObjectWrapper = MapObjectsRow.Add(ObjectName, NewObject<UMapObjectContainer>()->Add(MapObject, Type));

		MapObjectWrapper->OnLayerChange.AddLambda([&](UMapObjectContainer* Wrapper)
		{
			OnMapObjectChangeLayer.Broadcast(GetWorld()->GetMapName(), Wrapper);
		});

		if (bNotify)
			OnMapObjectAdd.Broadcast(MapName, MapObjectWrapper);
	}
}

void UMinimapController::RemoveMapObject_Internal(UMapObject* MapObject, EMapObjectType Type, const FString& MapName, bool bNotify)
{
	const auto& ObjectName = MapObject->GetUniqueName();
	auto& MapObjectsRow = GetMapObjects_Mutable(MapName);

	if (!ensure(MapObjectsRow.Contains(ObjectName))) return;

	const auto MapObjectWrapper = MapObjectsRow[ObjectName];
	check(MapObjectWrapper);

	MapObjectWrapper->Remove(Type);
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
