// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinimapLayerCollider.generated.h"

class UMapObjectComponent;
class UVisibilityCondition;


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

	/** Layer stack name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OwningLayerStack;

	/** Floor in level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Floor = 0;

#if WITH_EDITORONLY_DATA
	
	/** If layer has an irregular shape, use EditorOnly primitives (boxes) for bake layer bounds as simple data */
	UPROPERTY(EditAnywhere, Category = "Layer")
	TArray<AActor*> OptionalStaticLayerBounds;

	/** Layer visibility condition, baked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UVisibilityCondition* VisibilityCondition = nullptr;

#endif
	
	FString GetUniqueName() const;
	
protected:
	void OnOverlapPlayer();
	void OnEndOverlapPlayer();
};
