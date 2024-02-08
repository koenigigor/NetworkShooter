// Fill out your copyright notice in the Description page of Project Settings.


#include "MapLayerStack.h"

bool UVisibilityCondition::IsVisible_Implementation() const { return true; }

void UVisibilityCondition::Update()
{
	if (OwningLayerStack.IsValid())
	{
		OwningLayerStack->OnSublayerVisibilityUpdate.Broadcast(OwningLayerStack.Get());
	}
}

void UMapLayerStack::PostInitProperties()
{
	UObject::PostInitProperties();

	if(GetOuter() && GetOuter()->GetWorld())
	{
		Init();
	}
}

void UMapLayerStack::Init()
{
	UE_LOG(LogTemp, Display, TEXT("UMapLayerStack::Init"))
	
	for (const auto& [Floor, Sublayers] : Floors)
	{
		for (const auto& Sublayer : Sublayers.Sublayers)
		{
			Sublayer.VisibilityCondition->OwningLayerStack = MakeWeakObjectPtr(this);
		} 
	} 
}

bool UMapLayerStack::IsPointOnStack2D(FVector WorldLocation) const
{
	for (const auto& [Floor, Sublayers] : Floors)
	{
		for (const auto& Sublayer : Sublayers.Sublayers)
		{
			if (Sublayer.VisibilityCondition->IsVisible())
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

FVector UMapLayerStack::GetCenter() const
{
	// todo я очень очень не уверен в том что сделал
	
	FBox Box(EForceInit::ForceInit);

	for (const auto& [Floor, Sublayers] : Floors)
	{
		for (const auto& Sublayer : Sublayers.Sublayers)
		{
			if (Sublayer.VisibilityCondition->IsVisible())
			{
				for (const auto& Bound : Sublayer.Bounds)
				{
					FBoxSphereBounds NewBounds;
					NewBounds.Origin = Bound.Center; //Bound.Transform.TransformPosition(Bound.Center);
					NewBounds.BoxExtent = Bound.LocalExtend;
					NewBounds.TransformBy(Bound.Transform);

					Box += NewBounds.GetBox();
				}
			}
		} 
	}

	return Box.GetCenter();
}
