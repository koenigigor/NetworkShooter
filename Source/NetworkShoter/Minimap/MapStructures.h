// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "MapStructures.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MAP_DEFAULT);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MAP_HEAL);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MAP_AMMO);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MAP_CHEST);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MAP_MISC);

//todo 
FORCEINLINE FString GetMyMapName(UWorld* World)
{
	return World->GetMapName().Contains("UEDPIE") ? World->GetMapName().RightChop(9) : World->GetMapName();
}

/** Encapsulated main info about layer for clean view */
USTRUCT(BlueprintType)
struct NETWORKSHOTER_API FLayerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LayerGroup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sublayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Floor = 0;

	FString ToString() const;
	bool IsSameLayer(const FLayerInfo& Other) const;
	bool IsSameLayerGroup(const FLayerInfo& Other) const;
	bool IsEmpty() const;

	bool operator==(const FLayerInfo& Other) const
	{
		return Sublayer.Equals(Other.Sublayer);
	} 
};

enum EMapObjectType
{
	Baked,
	External,
	Runtime
};


/** Organized baked data for level */
USTRUCT(BlueprintType)
struct FBakedMapData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UMapLayersData> LayerData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UBakedMapObjects> BakedObjects;


};

