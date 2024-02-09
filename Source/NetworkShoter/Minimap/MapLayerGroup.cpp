// Fill out your copyright notice in the Description page of Project Settings.


#include "MapLayerGroup.h"

bool ULayerVisibilityCondition::IsVisible_Implementation() const { return true; }

void ULayerVisibilityCondition::Update()
{
	if (OwningGroup.IsValid())
	{
		OwningGroup->OnSublayerVisibilityUpdate.Broadcast(OwningGroup.Get());
	}
}

UWorld* ULayerVisibilityCondition::GetWorld() const
{
	if (GIsEditor && !GIsPlayInEditorWorld) return nullptr;
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}

///

void UMapLayerGroup::PostInitProperties()
{
	UObject::PostInitProperties();

	if (GetOuter() && GetOuter()->GetWorld())
	{
		Init();
	}
}

void UMapLayerGroup::Init()
{
	for (const auto& [Floor, Sublayers] : Floors)
	{
		for (const auto& Sublayer : Sublayers.Sublayers)
		{
			if (Sublayer.VisibilityCondition)
			{
				Sublayer.VisibilityCondition->OwningGroup = MakeWeakObjectPtr(this);
				Sublayer.VisibilityCondition->BeginPlay();
			}
		}
	}
}

bool UMapLayerGroup::IsPointInside2D(FVector WorldLocation) const
{
	for (const auto& [Floor, Sublayers] : Floors)
	{
		for (const auto& Sublayer : Sublayers.Sublayers)
		{
			if (Sublayer.IsVisible())
			{
				for (const auto& Bound : Sublayer.Bounds)
				{
					if (Bound.IsPointInside2D(WorldLocation))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

FVector UMapLayerGroup::GetCenter() const
{
	return GetBounds().GetCenter();
}

FBox UMapLayerGroup::GetBounds() const
{
	FBox Box(EForceInit::ForceInit);

	for (const auto& [Floor, Sublayers] : Floors)
	{
		for (const auto& Sublayer : Sublayers.Sublayers)
		{
			if (Sublayer.IsVisible())
			{
				for (const auto& Bound : Sublayer.Bounds)
				{
					FBoxSphereBounds NewBounds;
					NewBounds.Origin = Bound.Center;
					NewBounds.BoxExtent = Bound.LocalExtend;

					Box += NewBounds.GetBox().TransformBy(Bound.Transform);
				}
			}
		}
	}

	return Box;
}

///

UMapLayerGroup* UMapLayersData::GetGroupAtLocation2D(FVector WorldLocation)
{
	// find overlapped groups
	TArray<UMapLayerGroup*> Groups;
	for (auto Group : LayerGroups)
	{
		if (!Group->UniqueName.IsEmpty() && Group->IsPointInside2D(WorldLocation))
		{
			Groups.Add(Group);
		}
	}

	if (Groups.IsEmpty()) return nullptr;
	if (Groups.Num() == 1) return Groups[0];

	// find closest if multiple groups
	UMapLayerGroup* ClosestGroup = nullptr;
	float ClosestDistance = MAX_FLT;
	for (const auto Group : Groups)
	{
		const auto Distance = FVector::DistSquared2D(Group->GetCenter(), WorldLocation);
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestGroup = Group;
		}
	}

	return ClosestGroup;
}
