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
	//RemoveIcon();
	
	UMinimapController::Get(this)->UnregisterMapObject(this);
	
	Super::EndPlay(EndPlayReason);
}

void UMapObjectComponent::UpdateVisibility()
{
	if (OverlappedVolumes.IsEmpty())
	{
		//ShowIcon();
		return;
	}
	
	for (const auto& Volume : OverlappedVolumes)
	{
		if (Volume->IsLayerActive())
		{
			//ShowIcon();
			return;
		}
	}
		
	//RemoveIcon();
}


int32 UMapObjectComponent::GetLayer() const
{
	int32 Layer = 0;
	for (const auto& Volume : OverlappedVolumes) 
	{
		if (Volume->Layer > Layer)
		{
			Layer = Volume->Layer;
		}
	}
	
	return Layer;
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

	const auto bTopLayerChange = Volume->Layer > GetLayer();
	
	OverlappedVolumes.Add(Volume);
	Volume->AddIcon(this);

	UpdateVisibility();
	
	if (bTopLayerChange)
	{
		MapObject->Layer = GetLayer();
		MapObject->OnLayerChange.Broadcast(MapObject);
	}
}

void UMapObjectComponent::RemoveLayerVolume(UMinimapLayerCollider* Volume)
{
	OverlappedVolumes.Remove(Volume);
	Volume->RemoveIcon(this);

	UpdateVisibility();

	const auto bTopLayerChange = Volume->Layer > GetLayer();
	if (bTopLayerChange)
	{
		MapObject->Layer = GetLayer();
		MapObject->OnLayerChange.Broadcast(MapObject);
	}
}
