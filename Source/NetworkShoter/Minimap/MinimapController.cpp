// Fill out your copyright notice in the Description page of Project Settings.


#include "Minimap/MinimapController.h"

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



void UMinimapController::ShowIcon(UMapObjectComponent* Icon)
{
	if (VisibleMapObjects.Contains(Icon)) return;
	
	VisibleMapObjects.AddUnique(Icon);
	OnIconAdd.Broadcast(Icon);
}

void UMinimapController::RemoveIcon(UMapObjectComponent* Icon)
{
	if (!VisibleMapObjects.Contains(Icon)) return;
	
	OnIconRemove.Broadcast(Icon);
	VisibleMapObjects.Remove(Icon);
}
