// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NSInventoryComponent.generated.h"

class UNSItemInstance;
class UNSItemDefinition;
class UNSInventoryComponent;
struct FInventoryEntry;

UENUM(BlueprintType)
enum class EInventoryUpdateType : uint8
{
	NewItem,
	IncreaseCount,
	DecreaseCount,
	RemoveItem
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FInventoryItemDelegate, FInventoryEntry Item, EInventoryUpdateType UpdateType)

USTRUCT(BlueprintType)
struct FInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInventoryEntry() {};

	FInventoryEntry(UNSItemInstance* InItem, int32 InStackCount)
		: Item(InItem), StackCount(InStackCount) {};

	UPROPERTY(BlueprintReadOnly)
	UNSItemInstance* Item = nullptr;

	UPROPERTY(BlueprintReadOnly)
	int32 StackCount = 0;

	/** keep last count of item for calculate PostReplicatedChange() */
	UPROPERTY(NotReplicated)
	int32 LastObservedCount = 0;

	bool IsValid() const { return Item ? true : false; };
};


USTRUCT()
struct FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()
	FInventoryList() {};

	FInventoryList(UNSInventoryComponent* InOwningComponent)
		: InventoryComponent(InOwningComponent) {};

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInventoryEntry, FInventoryList>(Entries, DeltaParams, *this);
	}

	void AddEntry(TSubclassOf<UNSItemDefinition> Definition, int32 Count = 1);
	void AddEntry(UNSItemInstance* Item, int32 Count = 1);

	/** remove entry, array if item stack broken */
	bool RemoveEntry(TSubclassOf<UNSItemDefinition> Definition, TArray<FInventoryEntry>& RemovedEntries, int32 Count = 1, bool bExactCount = true);
	/** remove entry first starts from selected item */
	bool RemoveEntry(UNSItemInstance* Item, TArray<FInventoryEntry>& RemovedEntries, int32 Count = 1, bool bExactCount = true);

private:
	void RemoveEntryInternal(TIndexedContainerIterator<TArray<FInventoryEntry>, FInventoryEntry, int>& Iterator, int32& CountToRemove,
		TArray<FInventoryEntry>& RemovedEntries);

public:
	/** return first item with definition */
	FInventoryEntry FindItem(TSubclassOf<UNSItemDefinition> Definition);
	FInventoryEntry FindItem(UNSItemInstance* Instance);

	bool IsStackable(TSubclassOf<UNSItemDefinition> Definition) const;


	UNSItemInstance* CreateInstance(TSubclassOf<UNSItemDefinition> Definition);

	void AccelerationMapRemoveValue(TSubclassOf<UNSItemDefinition> Definition, int32 Value);

private:
	FInventoryEntry& FindItem_Mutable(TSubclassOf<UNSItemDefinition> Definition);

	friend UNSInventoryComponent;

	UPROPERTY()
	TArray<FInventoryEntry> Entries;

	//acceleration map for collect summary count of items by definition
	TMap<TSubclassOf<UNSItemDefinition>, int32> DefCountMap;

	UPROPERTY(NotReplicated)
	UNSInventoryComponent* InventoryComponent = nullptr;
};

template <>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UNSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNSInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	/** Create item from definition and add it in storage */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory")
	void AddItem_Definition(TSubclassOf<UNSItemDefinition> Definition, int32 Count = 1);

	/** Add existing item in storage */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory")
	void AddItem_Instance(UNSItemInstance* Item, int32 Count = 1);

	/** remove items from storage
	 * @bDestroy = Destroy removed items
	 * @bExactCount = if not have exact count of items, steel remove
	 * @Item = Starts from specified item
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory")
	bool RemoveItem(TSubclassOf<UNSItemDefinition> Definition, TArray<FInventoryEntry>& RemovedItems, int32 Count = 1, bool bDestroy = false,
		bool bExactCount = true);

	/** remove items, if count more than exist, remove other items with this definition starts */
	bool RemoveItem(UNSItemInstance* Item, TArray<FInventoryEntry>& RemovedItems, int32 Count = 1, bool bDestroy = false, bool bExactCount = true);

	bool RemoveItems(const TMap<TSubclassOf<UNSItemDefinition>, int32>& Items, TArray<FInventoryEntry>& RemovedItems, bool bDestroy = false,
		bool bExactCount = true);

	/** return first item with definition */
	UFUNCTION(BlueprintPure, Category="Inventory")
	FInventoryEntry FindItem(TSubclassOf<UNSItemDefinition> Definition);

	bool CheckItems(const TMap<TSubclassOf<UNSItemDefinition>, int32>& Items);

	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetTotalCount(TSubclassOf<UNSItemDefinition> Definition);

	/** GetCount in stack for specific instance */
	int32 GetCount(UNSItemInstance* Instance);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	TArray<FInventoryEntry> GetInventory();

	/**	NSEquipment component return item by this function */
	void AddItemFromEquipment(UNSItemInstance* Item);

	/** Item update/add/remove on inventory */
	FInventoryItemDelegate InventoryUpdate;

protected:
	/* read RemoveItem
	 *	BP function for not break overload */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Inventory")
	bool RemoveItemInstance(UNSItemInstance* Item, TArray<FInventoryEntry>& RemovedItems, int32 Count = 1, bool bDestroy = false, bool bExactCount = true);

private:
	UPROPERTY(Replicated)
	FInventoryList InventoryList;
};
