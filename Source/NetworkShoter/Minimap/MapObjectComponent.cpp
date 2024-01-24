// Fill out your copyright notice in the Description page of Project Settings.


#include "MapObjectComponent.h"

#include "CollisionChannels.h"
#include "MinimapLayerCollider.h"

#include "MinimapController.h"
#include "Components/Image.h"

UMapObjectComponent::UMapObjectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UMapObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	RefreshLayerVolumes(); //todo sometimes is overhead (movable icons)
	UpdateVisibility();
}

void UMapObjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveIcon();
	
	Super::EndPlay(EndPlayReason);
}

void UMapObjectComponent::UpdateVisibility()
{
	if (OverlappedVolumes.IsEmpty())
	{
		ShowIcon();
		return;
	}
	
	for (const auto& Volume : OverlappedVolumes)
	{
		if (Volume->IsLayerActive())
		{
			ShowIcon();
			return;
		}
	}
		
	RemoveIcon();
}

void UMapObjectComponent::ShowIcon()
{
	const auto Map = UMinimapController::Get(this);
	if (Map) Map->ShowIcon(this);	
}

void UMapObjectComponent::RemoveIcon()
{
	const auto Map = UMinimapController::Get(this);
	if (Map) Map->RemoveIcon(this);		
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
		OnLayerChange.Broadcast(this);
}

void UMapObjectComponent::RemoveLayerVolume(UMinimapLayerCollider* Volume)
{
	OverlappedVolumes.Remove(Volume);
	Volume->RemoveIcon(this);

	UpdateVisibility();

	const auto bTopLayerChange = Volume->Layer > GetLayer();
	if (bTopLayerChange)
		OnLayerChange.Broadcast(this);
}


UWidget* UMapObjectComponent::CreateIcon_Implementation()
{
	//create simple Image, override for custom widget
	const auto ImageWidget = NewObject<UImage>(this);
	ImageWidget->SetBrushResourceObject(Image);
	ImageWidget->SetDesiredSizeOverride(FVector2D(Size));
	ImageWidget->Brush.SetImageSize(FVector2D(Size));
	
	return ImageWidget;
}

