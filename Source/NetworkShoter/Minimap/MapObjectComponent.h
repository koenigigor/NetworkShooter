// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapObjectComponent.generated.h"

class UMinimapLayerCollider;
class UMapObject;

/**	Represent map object (icon or background),
 *	Specified icon parameters setup in MapObject class */
UCLASS(ClassGroup=(Minimap), meta=(BlueprintSpawnableComponent), Blueprintable)
class NETWORKSHOTER_API UMapObjectComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UMapObjectComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Check for overlapped layers, set icon layer to closest one */
	void UpdateLayer();

	/** Make trace for manual update OverlappedVolumes list */
	void RefreshLayerVolumes();

	/** Cache LayerVolume on overlap */
	void AddLayerVolume(UMinimapLayerCollider* Volume);
	void RemoveLayerVolume(UMinimapLayerCollider* Volume);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UMapObject* MapObject = nullptr;

protected:
	TSet<TWeakObjectPtr<UMinimapLayerCollider>> OverlappedVolumes;
};
