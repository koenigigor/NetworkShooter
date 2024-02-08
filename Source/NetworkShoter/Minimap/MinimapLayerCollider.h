// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinimapLayerCollider.generated.h"

class ULayerVisibilityCondition;


/**	Layers collider for map abjects, must be attached to trigger volume.
 *	Represent a map layer (1st floor, 2nd floor) */
UCLASS(ClassGroup=(Minimap), meta=(BlueprintSpawnableComponent), NotBlueprintable)
class NETWORKSHOTER_API UMinimapLayerCollider : public UActorComponent
{
	GENERATED_BODY()
public:
	UMinimapLayerCollider();
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Get asset unique name (same in pie and shipping builds) */
	FString GetUniqueName() const;

	/** The group this layer belongs to (Tower, Cave_01, etc)  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LayerGroup;

	/** Floor in layer group */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Floor = 0;

#if WITH_EDITORONLY_DATA

	/** Layer visibility condition (like "require discover area first") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	ULayerVisibilityCondition* VisibilityCondition = nullptr;

	/** If layer has an irregular shape, use box primitives (can be editor only) for bake layer bounds as simple data */
	UPROPERTY(EditAnywhere, Category = "Layer")
	TArray<AActor*> OptionalStaticLayerBounds;

#endif
};
