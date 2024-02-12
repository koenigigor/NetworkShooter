// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MapStructures.h"
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

	/** Notify visibility conditions update,
	 *	manual call after update condition */
	UFUNCTION(BlueprintCallable)
	void Update();

	UFUNCTION(BlueprintImplementableEvent)
	void BeginPlay();

	virtual UWorld* GetWorld() const override;

	TWeakObjectPtr<UMapLayerGroup> OwningGroup;
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

/** Single world map layer group
 *	Using for identify single group of layers (like castle or cave zone) */
UCLASS(DefaultToInstanced, EditInlineNew)
class NETWORKSHOTER_API UMapLayerGroup : public UObject
{
	GENERATED_BODY()
public:
	virtual void PostInitProperties() override;

	void Init();

	/** True if point inside this group of layers bounds */
	bool IsPointInside2D(FVector WorldLocation) const;

	/** Return center of layer stack bounds, use for identify closest layer stack, if found multiple */
	FVector GetCenter() const;

	/** Box bounds of all layers in group */
	FBox GetBounds() const;

	UPROPERTY(EditAnywhere)
	FString UniqueName;

	UPROPERTY(EditAnywhere)
	TMap<int32, FLayerSublayers> Floors;

	DECLARE_MULTICAST_DELEGATE_OneParam(FMapLayerStackDelegate, UMapLayerGroup* LayerGroup);
	FMapLayerStackDelegate OnSublayerVisibilityUpdate;
};


/** Data blueprint with baked information about all layers on map */
UCLASS(Blueprintable, Abstract)
class NETWORKSHOTER_API UMapLayersData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, meta=(TitleProperty="UniqueName"))
	TArray<UMapLayerGroup*> LayerGroups;

	/** Find closest overlapped layered location */
	UMapLayerGroup* GetGroupAtLocation2D(FVector WorldLocation);

	bool IsSublayerVisible(const FLayerInfo& LayerInfo);
};
