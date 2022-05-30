// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Equipment/EquipmentLib.h"
#include "NSEquipmentComponent.generated.h"
class UNSEquipmentInstance;
class UNSEquipmentDefinition;
class UNSItemInstance;
class UNSEquipmentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipDelegate, EEquipmentType, WeaponType, EEquipmentSlot, EquipmentSlot);


/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FNSEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FNSEquipmentEntry(){};
	FNSEquipmentEntry(UNSItemInstance* InItemInstance, UNSEquipmentInstance* InEquipmentInstance, EEquipmentSlot InSlot)
						: ItemInstance(InItemInstance), EquipmentInstance(InEquipmentInstance), Slot(InSlot){}
	
	UPROPERTY()
	UNSItemInstance* ItemInstance = nullptr;
	
	UPROPERTY()
	UNSEquipmentInstance* EquipmentInstance = nullptr;

	UPROPERTY()
	EEquipmentSlot Slot = EEquipmentSlot::None;
};

USTRUCT(BlueprintType)
struct FNSEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()
		
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FNSEquipmentEntry, FNSEquipmentList>(Entries, DeltaParams, *this);
	}

	void AddEntry(FNSEquipmentEntry Entry);

	/** Remove entry from array and return it */
	FNSEquipmentEntry RemoveEntry(const UNSEquipmentInstance* Instance);
	
	FNSEquipmentEntry GetEntryBySlot(EEquipmentSlot Slot) const;

private:
	friend UNSEquipmentComponent;
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FNSEquipmentEntry> Entries;

	TMap<UNSEquipmentInstance*, UNSItemInstance*> AccelerationMap;
	TMap<EEquipmentSlot, FNSEquipmentEntry> SlotMap;
};

template<>
struct TStructOpsTypeTraits<FNSEquipmentList> : public TStructOpsTypeTraitsBase2<FNSEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UNSEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNSEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	/** It not removed item from storage, don*t forget do it outside */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment")
	UNSEquipmentInstance* EquipItem(UNSItemInstance* Item);

	/** @DestroyItem if true destroy item instance and return nullptr
	 *	@bPutInInventory send item to owners inventory component */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment")
	UNSItemInstance* UnEquipItem(UNSEquipmentInstance* Item, bool bDestroy = false, bool bPutInInventory = true);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment")
	UNSItemInstance* UnEquipItemInSlot(EEquipmentSlot Slot, bool bDestroy = false, bool bPutInInventory = true);

	
	UFUNCTION(BlueprintCallable, Category="Equipment")
	FNSEquipmentEntry GetEquipmentBySlot(EEquipmentSlot Slot) { return EquipmentList.GetEntryBySlot(Slot); };

	UFUNCTION(BlueprintCallable, Category="Equipment")
	TArray<FNSEquipmentEntry> GetAllEquipment();

public:
	UPROPERTY(BlueprintAssignable)
	FEquipDelegate ItemEquip;
	
private:
	/** List of all equipped items */
	UPROPERTY(Replicated)
	FNSEquipmentList EquipmentList;

	EEquipmentSlot SuggestSlotToEquip(EEquipmentType Type);
};
