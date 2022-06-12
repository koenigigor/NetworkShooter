// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSInventoryComponent.h"
#include "Components/ActorComponent.h"
#include "NSItemSlider.generated.h"


class UNSItemInstance;
class UNSItemDefinition;
class UNSInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNSItemSliderUpdate);

UENUM()
enum ESliderType
{
	ByItemDefinition,
	ByItemInstance
};

USTRUCT(BlueprintType)
struct FItemOnSlider
{
	GENERATED_BODY()

	FItemOnSlider(){}
	FItemOnSlider(TSubclassOf<UNSItemDefinition> InDefinition, UNSItemInstance* InInstance, int32 InCount):Definition(InDefinition), Instance(InInstance), Count(InCount){}

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UNSItemDefinition> Definition = nullptr; //for Definition Slider

	UPROPERTY(BlueprintReadOnly)
	UNSItemInstance* Instance = nullptr; //for instance slider

	UPROPERTY(BlueprintReadOnly)
	int32 Count = 0;

	bool IsValid() const;
	void Invalidate();
};

/** [client only]
 *	Work with NSInventoryComponent
 *	adds item who satisfy requirements on slider to select them from ui
 *
 *	Derive it and override IsItemMustBeAddedOnSlider
 */
UCLASS(Blueprintable)
class NETWORKSHOTER_API UNSItemSlider : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNSItemSlider();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintPure)
	TArray<FItemOnSlider> GetSlider() { return Items; };

	UFUNCTION(BlueprintPure)
	TSubclassOf<UNSItemDefinition> GetSelectedDefinition() { return Items.IsValidIndex(ActiveSlot) ? Items[ActiveSlot].Definition : nullptr; }

	UFUNCTION(BlueprintPure)
	UNSItemInstance* GetSelectedInstance() { return Items.IsValidIndex(ActiveSlot) ? Items[ActiveSlot].Instance : nullptr; };

	UFUNCTION(BlueprintCallable)
	void Next(bool bUp = true, bool bOnlyValidSlot = true);

	UPROPERTY(BlueprintAssignable)
	FNSItemSliderUpdate SliderUpdateDelegate;

	UPROPERTY(BlueprintAssignable)
	FNSItemSliderUpdate ActiveSlotUpdateDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TEnumAsByte<ESliderType> SliderType;
    	
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	int32 MaxSlots = 3;
    
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	bool bChangeActiveSlotOnInvalidate = true;
    
	/** dont remove slot if we remove item (like for equip weapon)
	 *	*May be issue if we drop item or etc
	 *	*need enum in Inventory->ItemUpdate delegate purpose (equip, drop, sell...) */
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	bool bCacheActiveRemovedSlot = false;

	
protected:
	/** Specific action on select slot */
	UFUNCTION(BlueprintNativeEvent)
	void OnSlotSelected(int32 PreviousSlot);
	
	UFUNCTION(BlueprintNativeEvent)
	bool IsItemMustBeAddedOnSlider(UNSItemInstance* Item);
	
	void OnItemUpdate(FInventoryEntry Entry);

	int32 FindIndexOnSlider(const UNSItemInstance* Item) const;

	int32 GetNewCount(const FInventoryEntry& Entry) const;

	int32 GetFirstFreeIndex() const;	
	
	TArray<FItemOnSlider> Items;
	
	UPROPERTY()
	UNSInventoryComponent* Inventory;
	
	int32 CachedSlot = -1;
	
	UPROPERTY(BlueprintReadOnly)
	int32 ActiveSlot = -1;
};
