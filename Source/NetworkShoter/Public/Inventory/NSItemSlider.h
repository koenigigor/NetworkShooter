// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NSItemSlider.generated.h"


class UNSItemInstance;
class UNSItemDefinition;
class UNSInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNSItemSliderUpdate);

USTRUCT(BlueprintType)
struct FItemOnSlider
{
	GENERATED_BODY()

	FItemOnSlider(){}
	FItemOnSlider(TSubclassOf<UNSItemDefinition> InDefinition, int32 InCount):Definition(InDefinition), Count(InCount){}

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UNSItemDefinition> Definition = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 Count = 0;
};

/** client side component
 *	work with NSInventoryComponent
 *	adds item who satisfy requirements on slider to select them from ui
 *
 *	Derive it and override IsItemMustBeAddedOnSlider
 */
UCLASS(Blueprintable)//ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UNSItemSlider : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNSItemSlider();

	UFUNCTION(BlueprintPure)
	TArray<FItemOnSlider> GetSlider() { return Items; };

	UFUNCTION(BlueprintPure)
	TSubclassOf<UNSItemDefinition> GetSelectedItem();

	UFUNCTION(BlueprintCallable)
	void Next(bool bUp = true);

	UPROPERTY(BlueprintAssignable)
	FNSItemSliderUpdate SliderUpdateDelegate;

	UPROPERTY(BlueprintAssignable)
	FNSItemSliderUpdate ActiveSlotUpdateDelegate;
protected:
	
	virtual void BeginPlay() override;

	void OnItemAdded(UNSItemInstance* Item, int32 Count);
	void OnItemRemoved(UNSItemInstance* Item, int32 Count);

	UFUNCTION(BlueprintNativeEvent)
	bool IsItemMustBeAddedOnSlider(UNSItemInstance* Item);

	TArray<FItemOnSlider> Items;
	
	UPROPERTY()
	UNSInventoryComponent* Inventory;

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	int32 MaxSlots = 3;
	
	UPROPERTY(BlueprintReadOnly)
	int32 ActiveSlot = -1;

private:
	int32 FindIndexOnSlider(TSubclassOf<UNSItemDefinition> Definition) const;
};
