// Fill out your copyright notice in the Description page of Project Settings.


#include "Area/AreaVolume.h"


UAreaVolume::UAreaVolume()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAreaVolume::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()->bGenerateOverlapEventsDuringLevelStreaming)
	{
		UE_LOG(LogTemp, Warning, TEXT("please enable bGenerateOverlapEventsDuringLevelStreaming on %s"), *GetOwner()->GetName())
	}

	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	GetOwner()->OnActorEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

void UAreaVolume::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (const auto AreaListener = OtherActor->FindComponentByClass<UAreaListenerComponent>())
	{
		OverlappedListeners.Add(AreaListener);
		AreaListener->AddOverlappedArea(this);
	}
}

void UAreaVolume::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (const auto AreaListener = OtherActor->FindComponentByClass<UAreaListenerComponent>())
	{
		OverlappedListeners.Remove(AreaListener);
		AreaListener->RemoveOverlappedArea(this);
	}	
}


/* --------------------- */


UAreaListenerComponent::UAreaListenerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UAreaVolume* UAreaListenerComponent::GetCurrentVolume() const
{
	//find volume with high priority
	UAreaVolume* TopVolume = nullptr;
	for (const auto& Area : OverlappedAreaVolumes)
	{
		if (Area.IsValid() && (!TopVolume || Area->Priority > TopVolume->Priority))
		{
			TopVolume = Area.Get();
		}
	}

	return TopVolume;
}

void UAreaListenerComponent::GetActorsInCurrentArea(TArray<AActor*>& OutActors, bool bOnlyHighPriority) const
{
	const auto Area = GetCurrentVolume();
	if (!Area) return;

	for (const auto& Listener : Area->GetOverlappedListeners())
	{
		if (Listener.IsValid())
		{
			if (!bOnlyHighPriority || Area == Listener->GetCurrentVolume())
			{
				OutActors.Add(Listener->GetOwner());
			}
		}
	}
}

void UAreaListenerComponent::AddOverlappedArea(UAreaVolume* Area)
{
	if (OverlappedAreaVolumes.Contains(Area)) return;

	const auto PrevArea = GetCurrentVolume();
	OverlappedAreaVolumes.Add(Area);
	const auto NewArea = GetCurrentVolume();

	if (PrevArea != NewArea)
	{
		constexpr float NotifyAreaChangeDelay = 5.f;
		GetWorld()->GetTimerManager().SetTimer(NotifyAreaChangeTimer, this, &ThisClass::AreaChanged, NotifyAreaChangeDelay, false);
	}
}

void UAreaListenerComponent::RemoveOverlappedArea(UAreaVolume* Area)
{
	if (!OverlappedAreaVolumes.Contains(Area)) return;

	const auto PrevArea = GetCurrentVolume();
	OverlappedAreaVolumes.Remove(Area);
	const auto NewArea = GetCurrentVolume();

	if (PrevArea != NewArea)
	{
		if (bNotifyEnterOnExit)
		{
			constexpr float NotifyAreaChangeDelay = 5.f;
			GetWorld()->GetTimerManager().SetTimer(NotifyAreaChangeTimer, this, &ThisClass::AreaChanged, NotifyAreaChangeDelay, false);
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(NotifyAreaChangeTimer);
		}
	}
}

void UAreaListenerComponent::AreaChanged()
{
	OnAreaChange.Broadcast();
}

