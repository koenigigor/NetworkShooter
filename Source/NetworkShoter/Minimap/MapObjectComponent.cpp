// Fill out your copyright notice in the Description page of Project Settings.


#include "MapObjectComponent.h"

#include "CollisionChannels.h"
#include "MapObject.h"
#include "MinimapLayerCollider.h"
#include "MinimapController.h"

UMapObjectComponent::UMapObjectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//default map object variant
	MapObject = CreateDefaultSubobject<UMapObjectSimpleImage>("MapObject");

	//todo not find any "Event Construct" for auto update parameters, layers
	//MapObject->InitialLocation = GetOwner()->GetActorLocation();
}


void UMapObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	UMinimapController::Get(this)->RegisterMapObject(this);
	
	RefreshLayerVolumes();
	UpdateVisibility();
}

void UMapObjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UMinimapController::Get(this)->UnregisterMapObject(this);
	
	Super::EndPlay(EndPlayReason);
}

void UMapObjectComponent::UpdateVisibility()
{
	const auto PreviousLayer = MapObject->GetLayer();
	
	if (OverlappedVolumes.IsEmpty())
	{
		MapObject->LayerInfo = {"", "", 0};
	}
	else
	{
		UMinimapLayerCollider* ClosestLayer = nullptr;
		float ClosestDistance = MAX_FLT;
		for (const auto& Layer : OverlappedVolumes)
		{
			const auto Distance = Layer->GetOwner()->GetDistanceTo(GetOwner());
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestLayer = Layer.Get();
			}
		}

		MapObject->LayerInfo = { ClosestLayer->OwningLayerStack, ClosestLayer->GetUniqueName(), ClosestLayer->Floor };
	}
	
	if (!PreviousLayer.Equals(MapObject->GetLayer()))
	{
		MapObject->OnLayerChange.Broadcast(MapObject);

		//if is player, notify player layer
		if (const auto Pawn = GetOwner<APawn>(); Pawn && Pawn->IsLocallyControlled())
		{
			const auto MapController = UMinimapController::Get(this);
			MapController->SetPlayerLayer(MapObject->LayerInfo);
		}
	}
}

void UMapObjectComponent::RefreshLayerVolumes()
{
	// Clear current volumes
	for (const auto& Layer : OverlappedVolumes.Array())
	{
		RemoveLayerVolume(Layer.Get());
	}
	OverlappedVolumes.Empty();

	// Find overlapped LayerVolumes
	TArray<FOverlapResult> Hits;
	FComponentQueryParams Params(SCENE_QUERY_STAT(UpdateMinimapLauerVolumes));
	Params.bIgnoreBlocks = true;
	GetWorld()->OverlapMultiByChannel(
		Hits,
		GetOwner()->GetActorLocation(),
		FQuat::Identity,
		CHANNEL_LOCATION,
		FCollisionShape::MakeSphere(0.f),
		Params);

	for (const FOverlapResult& Hit : Hits)
	{
		const auto LayerCollider = Hit.GetActor()->FindComponentByClass<UMinimapLayerCollider>();
		if (LayerCollider)
		{
			AddLayerVolume(LayerCollider);
		}
	}
}

void UMapObjectComponent::AddLayerVolume(UMinimapLayerCollider* Volume)
{
	if (OverlappedVolumes.Contains(Volume)) return;
	
	OverlappedVolumes.Add(Volume);

	UpdateVisibility();
}

void UMapObjectComponent::RemoveLayerVolume(UMinimapLayerCollider* Volume)
{
	OverlappedVolumes.Remove(Volume);

	UpdateVisibility();
}
