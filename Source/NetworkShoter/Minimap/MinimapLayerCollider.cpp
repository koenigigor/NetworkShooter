// Fill out your copyright notice in the Description page of Project Settings.


#include "Minimap/MinimapLayerCollider.h"

#include "MapObjectComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogMinimapCollider, All, All);

UMinimapLayerCollider::UMinimapLayerCollider()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMinimapLayerCollider::BeginPlay()
{
	Super::BeginPlay();
	
	//if (!GetOwner()->bGenerateOverlapEventsDuringLevelStreaming)
	//{
	//	UE_LOG(LogMinimapCollider, Warning, TEXT("Can be issues, enable bGenerateOverlapEventsDuringLevelStreaming on actor %s"), *GetOwner()->GetName())
	//}

	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	GetOwner()->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

void UMinimapLayerCollider::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// If player activate layer
	const auto Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn->IsPlayerControlled() && Pawn->IsLocallyControlled())
	{
		ActivateLayer();
	}	

	// Notify register icon on overlap, (in Collier overlap, because icon actor can have lot overlaps, and it'll be not optimized performance)
	if (const auto Icon = OtherActor->FindComponentByClass<UMapObjectComponent>())
    {
		Icon->AddLayerVolume(this);
    }	
}

void UMinimapLayerCollider::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Notify unregister icon on end overlap
	if (const auto Icon = OtherActor->FindComponentByClass<UMapObjectComponent>())
	{
		Icon->RemoveLayerVolume(this);
	}

	// If player deactivate layer
	const auto Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn->IsPlayerControlled() && Pawn->IsLocallyControlled())
	{
		DeactivateLayer();
	}		
}

void UMinimapLayerCollider::ActivateLayer()
{
	UE_LOG(LogMinimapCollider, Display, TEXT("Activate %s, Layer = %d"), *GetName(), Layer)

	bLayerActive = true;

	for (const auto& Icon : OverlappedIcons)
	{
		if (Icon.IsValid())
			Icon->UpdateVisibility();
	}
}

void UMinimapLayerCollider::DeactivateLayer()
{
	UE_LOG(LogMinimapCollider, Display, TEXT("Deactivate %s, Layer = %d"), *GetName(), Layer)

	bLayerActive = false;

	for (const auto& Icon : OverlappedIcons)
	{
		if (Icon.IsValid())
			Icon->UpdateVisibility();
	}
}


void UMinimapLayerCollider::AddIcon(UMapObjectComponent* Icon)
{
	OverlappedIcons.Add(Icon);
}

void UMinimapLayerCollider::RemoveIcon(UMapObjectComponent* Icon)
{
	OverlappedIcons.Remove(Icon);
}



