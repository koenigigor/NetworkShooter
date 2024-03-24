// Fill out your copyright notice in the Description page of Project Settings.


#include "MapLayerGroup.h"

#include "MapVisibilityCondition.h"


bool FLayerSublayer::IsVisible() const
{
	return VisibilityCondition ? VisibilityCondition->IsVisible() : true;
}

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
				Sublayer.VisibilityCondition->OnVisibilityUpdate.BindUObject(this, &ThisClass::UpdateVisibilityHandle);
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

void UMapLayerGroup::UpdateVisibilityHandle()
{
	OnSublayerVisibilityUpdate.Broadcast(this);
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

bool UMapLayersData::IsSublayerVisible(const FLayerInfo& LayerInfo)
{
	if (LayerInfo.IsEmpty()) return true; // Ground layer always visible
	
	for (const auto Group : LayerGroups)
	{
		if (Group->UniqueName.Equals(LayerInfo.LayerGroup))
		{
			const auto Sublayers = Group->Floors.Find(LayerInfo.Floor);
			if (Sublayers)
			{
				for (auto Sublayer : Sublayers->Sublayers)
				{
					if (Sublayer.UniqueName.Equals(LayerInfo.Sublayer))
					{
						return Sublayer.IsVisible();
					}
				}
			}
		}
	}
	
	return false;
}
