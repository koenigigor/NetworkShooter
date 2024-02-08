// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MapLayerGroup.generated.h"

class UMapLayerGroup;

USTRUCT(BlueprintType)
struct FLayerBounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector Center = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	FVector LocalExtend = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	FTransform Transform;

	bool IsPointInside2D(FVector WorldPosition) const
	{
		const auto LocalPosition = Transform.InverseTransformPosition(WorldPosition) + Center;
		const auto Box = FBox(-LocalExtend, LocalExtend);
		return Box.IsInsideXY(LocalPosition);
	}
};

/** Visibility condition for sublayer,
 *	if IsVisible() return false, this part of layer will be hidden (like area not discover) */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable)
class NETWORKSHOTER_API ULayerVisibilityCondition : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	bool IsVisible() const;

	/** Update visibility conditions,
	 *	manual call after update condition */
	UFUNCTION(BlueprintCallable)
	void Update();

	UFUNCTION(BlueprintImplementableEvent)
	void BeginPlay();

	TWeakObjectPtr<UMapLayerGroup> OwningLayerStack;
};

/** Part of layer, can be enabled by condition (this part of floor not explored, etc)
 *	if subfloor disabled it will be hidden on global map (include layer collision) */
USTRUCT(BlueprintType)
struct FLayerSublayer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString UniqueName;

	/** Static sublayer bounds */
	UPROPERTY(EditAnywhere)
	TArray<FLayerBounds> Bounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	ULayerVisibilityCondition* VisibilityCondition = nullptr;

	bool IsVisible() const { return VisibilityCondition ? VisibilityCondition->IsVisible() : true; };
};

/** Group of sublayers in single floor */
USTRUCT(BlueprintType)
struct FLayerSublayers
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FLayerSublayer> Sublayers;
};

/** Single world map layer stack
 *	Using for identify single group of layers (like castle or cave) */
UCLASS(DefaultToInstanced, EditInlineNew)
class NETWORKSHOTER_API UMapLayerGroup : public UObject
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;

	UPROPERTY(EditAnywhere)
	FString UniqueName;

	UPROPERTY(EditAnywhere)
	TMap<int32, FLayerSublayers> Floors;

	void Init();


	/** True if point overlap layer stack bounds */
	bool IsPointOnStack2D(FVector WorldLocation) const;

	/** Return center of layer stack bounds, use for identify closest layer stack, if found multiple */
	FVector GetCenter() const;

	DECLARE_MULTICAST_DELEGATE_OneParam(FMapLayerStackDelegate, UMapLayerGroup* MapLayerStack);
	FMapLayerStackDelegate OnSublayerVisibilityUpdate;
};


/** Data blueprint with baked information about all layers on map */
UCLASS(Blueprintable)
class NETWORKSHOTER_API UMapLayersData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UMapLayerGroup*> LayerStacks;
};
