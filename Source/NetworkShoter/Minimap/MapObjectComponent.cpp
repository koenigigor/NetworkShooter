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
}


void UMapObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto MapController = UMinimapController::Get(this);
	if (MapController) MapController->RegisterMapObject(this);

	RefreshLayerVolumes();
	UpdateLayer();
}

void UMapObjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const auto MapController = UMinimapController::Get(this);
	if (MapController) MapController->UnregisterMapObject(this);

	Super::EndPlay(EndPlayReason);
}

void UMapObjectComponent::UpdateLayer()
{
	const auto PreviousSublayer = MapObject->GetSublayer();

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

		MapObject->LayerInfo = {ClosestLayer->LayerGroup, ClosestLayer->GetUniqueName(), ClosestLayer->Floor};
	}

	if (!PreviousSublayer.Equals(MapObject->GetSublayer()))
	{
		MapObject->OnLayerChange.Broadcast(MapObject);

		//if is player icon, notify player layer
		if (const auto Pawn = GetOwner<APawn>(); Pawn && Pawn->IsPlayerControlled())
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

	// Set new volumes
	for (const FOverlapResult& Hit : Hits)
	{
		const auto LayerCollider = Hit.GetActor()->FindComponentByClass<UMinimapLayerCollider>();
		if (LayerCollider && !OverlappedVolumes.Contains(LayerCollider))
		{
			OverlappedVolumes.Add(LayerCollider);
		}
	}

	UpdateLayer();
}

void UMapObjectComponent::AddLayerVolume(UMinimapLayerCollider* Volume)
{
	if (OverlappedVolumes.Contains(Volume)) return;

	OverlappedVolumes.Add(Volume);

	UpdateLayer();
}

void UMapObjectComponent::RemoveLayerVolume(UMinimapLayerCollider* Volume)
{
	OverlappedVolumes.Remove(Volume);

	UpdateLayer();
}
