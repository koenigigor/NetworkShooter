// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapObjectComponent.generated.h"

class UMinimapLayerCollider;
class UMapObject;

/** Represent map icon object
 *	Specify your icon in MapObject class
 *	for represent background image or icon
 *	Keep overlapped layer volumes */
UCLASS(ClassGroup=(Minimap), meta=(BlueprintSpawnableComponent), Blueprintable )
class NETWORKSHOTER_API UMapObjectComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UMapObjectComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Toggle visibility if any layer volume active */
	void UpdateVisibility();	

	/** Make trace for manual update OverlappedVolumes list */
	void RefreshLayerVolumes();

	/** Update icon location on map widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIcon"))
	bool bMovable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UMapObject* MapObject = nullptr;

	/** Cache LayerVolume on overlap */
	void AddLayerVolume(UMinimapLayerCollider* Volume);
	void RemoveLayerVolume(UMinimapLayerCollider* Volume);

protected:
	TSet<TWeakObjectPtr<UMinimapLayerCollider>> OverlappedVolumes;
};


