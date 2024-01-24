// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NSInventorySlider.generated.h"

enum class EInventoryUpdateType : uint8;
class UNSItemDefinition;
class UNSItemInstance;
class UNSInventoryComponent;
struct FInventoryEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventorySliderUpdateDelegate);

DEFINE_LOG_CATEGORY_STATIC(LogInventorySlider, All, All);

USTRUCT(BlueprintType)
struct FSliderEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UNSItemDefinition> ItemDefinition;

	UPROPERTY(BlueprintReadOnly)
	UNSItemInstance* AssignedInstance = nullptr;

	bool IsValid() const;
};

/**
 * Base class for item slider, (for ui organisation)
 * add reference important item for fast use it
 * (like 5-8 keys in game)
 *
 * for use override:
 * - UseItem()
 * - IsItemMustBeAdd()
 */
UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class NETWORKSHOTER_API UNSInventorySlider : public UActorComponent
{
	GENERATED_BODY()
public:
	UNSInventorySlider();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "InventorySlider")
	void AddItemInSlot(int32 Slot, TSubclassOf<UNSItemDefinition> Definition, UNSItemInstance* AssignedInstance = nullptr);

	UFUNCTION(BlueprintCallable, Category = "InventorySlider")
	void RemoveItemFromSlot(int32 Slot);

	/** Find next slot, and set it as active */
	UFUNCTION(BlueprintCallable, Category = "InventorySlider")
	void NextSlot(bool bUp = true);

	UFUNCTION(BlueprintCallable, Category = "InventorySlider")
	void UseItemInSlot(int32 Slot);

	UFUNCTION(BlueprintPure, Category = "InventorySlider")
	int32 GetSelectedSlot() const;

	UFUNCTION(BlueprintPure, Category = "InventorySlider")
	FSliderEntry GetSelectedItem() const;

	//----------
	UFUNCTION(BlueprintPure, Category = "InventorySlider")
	int32 GetCount(const FSliderEntry& Entry) const;

	/** Can keep no more owned items, use GetCount() for ensure */
	UFUNCTION(BlueprintPure, Category = "InventorySlider")
	TArray<FSliderEntry> GetSlots() const { return Entries; }

	/** Can keep no more owned items, check count for ensure */
	UFUNCTION(BlueprintPure, Category = "InventorySlider", meta = (ReturnDisplayName = "Entry"))
	FSliderEntry GetSlot(int32 Slot, int32& Count);
	FSliderEntry GetSlot(int32 Slot);

	/** Get item instance stored in slot, if slider use total count, return first item in storage,
	 *	if keep ref dropped item (cache slot), return nullptr */
	UFUNCTION(BlueprintCallable, Category = "InventorySlider")
	UNSItemInstance* GetStoredInstance(int32 Slot);

	//----------

	UPROPERTY(BlueprintAssignable)
	FInventorySliderUpdateDelegate OnUpdate;

	UPROPERTY(BlueprintAssignable)
	FInventorySliderUpdateDelegate OnUpdateActiveSlot;

protected:
	/** Init slot with selected definition, for clear slot init this with nullptr definition */
	void InitSlot(int32 Slot, TSubclassOf<UNSItemDefinition> Definition, UNSItemInstance* AssignedInstance = nullptr);

	UFUNCTION(BlueprintNativeEvent)
	void UseItem(const FSliderEntry& Item);

	void OnInventoryUpdate(FInventoryEntry InventoryEntry, EInventoryUpdateType UpdateType);

	UFUNCTION(BlueprintNativeEvent)
	bool IsItemMustBeAdd(TSubclassOf<UNSItemDefinition> Item) const;
	bool ItemDefAlreadyOnSlider(const FInventoryEntry& InventoryEntry) const;
	int32 GetFreeSlot(const FInventoryEntry& InventoryEntry) const;

protected:
	/** Use item stack count, or total items count in inventory */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	bool bUseTotalCount = false;

	/** Clear slot if item remove from inventory */
	//UPROPERTY(EditDefaultsOnly, Category = "Setup")
	//bool bClearEmptySlot = false;

	/** NextSlot() search next valid slot */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	bool bIterateOnlyValidSlot = true;

	/** Auto add item on slider, when inventory adds item,
	 *	need override IsItemMustBeAdd() */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	bool bBindToInventoryUpdate = true;

	/** UseItem on scroll NextSlot() */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	bool bUseItemOnActivateSlot = false;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int32 NumSlots = 4.f;

	int32 ActiveIndex = INDEX_NONE;


protected:
	UPROPERTY()
	UNSInventoryComponent* Inventory = nullptr;

	TArray<FSliderEntry> Entries;
};
