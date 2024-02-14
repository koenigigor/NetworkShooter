// Fill out your copyright notice in the Description page of Project Settings.


#include "MapStructures.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_MAP_ROOT, "Map");
UE_DEFINE_GAMEPLAY_TAG(TAG_MAP_DEFAULT, "Map.None");
UE_DEFINE_GAMEPLAY_TAG(TAG_MAP_HEAL, "Map.Heal");
UE_DEFINE_GAMEPLAY_TAG(TAG_MAP_AMMO, "Map.Ammo");
UE_DEFINE_GAMEPLAY_TAG(TAG_MAP_CHEST, "Map.Chest");
UE_DEFINE_GAMEPLAY_TAG(TAG_MAP_MISC, "Map.Misc");

FString FLayerInfo::ToString() const
{
	if (LayerGroup.IsEmpty()) return "Is Ground";
	
	return "Group:" + LayerGroup + ", Floor: " + FString::FromInt(Floor) + ", Sublayer: " + Sublayer;
}

bool FLayerInfo::IsSameLayer(const FLayerInfo& Other) const
{
	return LayerGroup.Equals(Other.LayerGroup) && Floor == Other.Floor;
}
bool FLayerInfo::IsSameLayerGroup(const FLayerInfo& Other) const
{
	return LayerGroup.Equals(Other.LayerGroup);
}

bool FLayerInfo::IsEmpty() const
{
	return LayerGroup.IsEmpty();
}
