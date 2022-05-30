// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NSWeaponSliderComponent.generated.h"


class UNSInventoryComponent;
class UNSEquipmentComponent;
class UNSItemInstance;

/**
 * just TArray<UNSItemInstance> and replicate subobjects not work
 */

USTRUCT(BlueprintType)
struct FSlotEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FSlotEntry(){};
	FSlotEntry(UNSItemInstance* InItem):Item(InItem){};
	
	UPROPERTY()
	UNSItemInstance* Item = nullptr;
};

USTRUCT()
struct FItemSlots : public FFastArraySerializer
{
	GENERATED_BODY()

	void AddItem(UNSItemInstance* Item);
	void RemoveIndex(int32 Index);

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize){};
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize){};
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize){};
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FSlotEntry, FItemSlots>(Entries, DeltaParams, *this);
	}
	
	UPROPERTY()
	TArray<FSlotEntry> Entries;

	UNSItemInstance*& operator[](int32 Index)
	{
		ensure(Entries.IsValidIndex(Index));
		return Entries[Index].Item;
	}

	int32 Num() const {return Entries.Num(); }
};

template<>
struct TStructOpsTypeTraits<FItemSlots> : public TStructOpsTypeTraitsBase2<FItemSlots>
{
	enum { WithNetDeltaSerializer = true };
};


/**
 * On weapon added in inventory intercept them and save them if has place
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UNSWeaponSliderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNSWeaponSliderComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	int32 MaxSlots = 3;

	UFUNCTION(BlueprintCallable, Server, Unreliable, Category="Equipment")
	void NextWeapon(bool bUp = true);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment")
	UNSItemInstance* RemoveWeaponFromSlider(bool bAddInStorage = true);

	UFUNCTION(BlueprintPure, Category="Equipment")
	TArray<UNSItemInstance*> GetSlider();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void AddItemOnSlider(UNSItemInstance* Item);

	UPROPERTY(Replicated)
	int32 ActiveSlot = -1;

	UPROPERTY(Replicated)
	//TArray<UNSItemInstance*> Slots;
	FItemSlots Slots;

	/** keep previous removed weapon for not re add it on remove */
	UPROPERTY()
	UNSItemInstance* RemovedWeapon = nullptr;

	UPROPERTY()
	UNSEquipmentComponent* Equipment = nullptr;
	UPROPERTY()
	UNSInventoryComponent* Inventory = nullptr;
};
