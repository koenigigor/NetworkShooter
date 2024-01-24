// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinimapLayerCollider.generated.h"

class UMapObjectComponent;


/**	Component for trigger volume.
 *	Represent a map layer (ground, 1st floor, 2nd floor).
 *	Register in icon on overlap.
 *	Activate layer on overlap with player. */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class NETWORKSHOTER_API UMinimapLayerCollider : public UActorComponent
{
	GENERATED_BODY()
public:	
	UMinimapLayerCollider();
	virtual void BeginPlay() override;
	UFUNCTION() void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION() void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	FORCEINLINE bool IsLayerActive() const { return bLayerActive; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Layer = 0;

	/** Icons and Backgrounds register self in volume */
	void AddIcon(UMapObjectComponent* Icon);
	void RemoveIcon(UMapObjectComponent* Icon);
	
protected:
	void ActivateLayer();
	void DeactivateLayer();
	
	//activate when character overlap
	bool bLayerActive = false;

	/** Cashed backgrounds and icons for easiest access */
	TArray<TWeakObjectPtr<UMapObjectComponent>> OverlappedIcons;
};
