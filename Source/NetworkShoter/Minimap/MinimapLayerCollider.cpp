// Fill out your copyright notice in the Description page of Project Settings.


#include "Minimap/MinimapLayerCollider.h"

#include "MapObjectComponent.h"

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
	// Notify icon on begin overlap
	if (const auto Icon = OtherActor->FindComponentByClass<UMapObjectComponent>())
	{
		Icon->AddLayerVolume(this);
	}
}

void UMinimapLayerCollider::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Notify icon on end overlap
	if (const auto Icon = OtherActor->FindComponentByClass<UMapObjectComponent>())
	{
		Icon->RemoveLayerVolume(this);
	}
}

FString UMinimapLayerCollider::GetUniqueName() const
{
	//get unique name in format ActorName.ComponentName  (GetUniqueID() has different results in shipping)
	FString OutString;
	GetPathName(GetOuter()->GetOuter(), OutString);
	return OutString;
}
