﻿// Fill out your copyright notice in the Description page of Project Settings.

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


//class UWidget; //todo custom class

/** Map object data for bake/json export */
USTRUCT(BlueprintType)
struct FBakedMapObjectData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIcon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Size = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag FilterCategory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=( DisplayThumbnail="true", DisplayName="Image", AllowedClasses="/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"))
	TSoftObjectPtr<UObject> Image;

	UPROPERTY()
	int32 WidgetPriority = 0;

	// custom icon info
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TSoftObjectPtr<UWidget> CustomWidgetClass;

	UPROPERTY()
	TArray<uint8> CustomWidgetBinaryData;
};

/** Map layer data for bake/json export */
USTRUCT(BlueprintType)
struct FBakedMapLayerData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform = FTransform::Identity;

	// Center of collision in Local space
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Center = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Extend = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Layer = -999;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBakedMapObjectData> MapObjects;
};

/** Level and its baked map data */
USTRUCT(BlueprintType)
struct FBakedMapData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FBakedMapLayerData> Layers;

	/** return array of layers by 2d world position */
	TArray<FBakedMapLayerData*> GetLayersAtPosition(FVector WorldPosition);
};

