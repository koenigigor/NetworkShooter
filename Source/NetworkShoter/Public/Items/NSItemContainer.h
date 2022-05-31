// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractInterface.h"
#include "NSItemContainer.generated.h"

class UNSItemDefinition;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class NETWORKSHOTER_API ANSItemContainer : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ANSItemContainer();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UNSItemDefinition> ItemToPick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count = 1;

	virtual bool InteractWithPawn_Implementation(APawn* InteractWith) override;

protected:
	UFUNCTION(BlueprintNativeEvent)
	bool CanInteract();
	
	UFUNCTION(BlueprintNativeEvent)
	void ItemGathered();
};
