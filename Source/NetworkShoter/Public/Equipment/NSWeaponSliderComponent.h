// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NSWeaponSliderComponent.generated.h"

class UNSInventoryComponent;
class UNSEquipmentComponent;
class UNSItemInstance;
class UNSWeaponSliderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSliderUpdate);

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

	FItemSlots(){};
	FItemSlots(UNSWeaponSliderComponent* InSlider):OwningSlider(InSlider){};

	void AddItem(UNSItemInstance* Item);
	void RemoveIndex(int32 Index);

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
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

	UPROPERTY(NotReplicated)
	UNSWeaponSliderComponent* OwningSlider = nullptr;
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

	UPROPERTY(BlueprintAssignable)
	FSliderUpdate SliderUpdate;

	UPROPERTY(BlueprintAssignable)
	FSliderUpdate ActiveSlotChange;

	UFUNCTION(BlueprintCallable, Server, Unreliable, Category="Equipment")
	void NextWeapon(bool bUp = true);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Equipment")
	UNSItemInstance* RemoveWeaponFromSlider(bool bAddInStorage = true);

	UFUNCTION(BlueprintPure, Category="Equipment")
	TArray<UNSItemInstance*> GetSlider();

	int32 GetActiveSlot() const { return ActiveSlot; }
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void AddItemOnSlider(UNSItemInstance* Item);

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	int32 MaxSlots = 3;
	
	UPROPERTY(ReplicatedUsing="OnRep_ActiveSlot", BlueprintReadOnly)
	int32 ActiveSlot = -1;

	UPROPERTY(Replicated)
	FItemSlots Slots;

	/** keep previous removed weapon for not re add it on remove */
	UPROPERTY()
	UNSItemInstance* RemovedWeapon = nullptr;

	UPROPERTY()
	UNSEquipmentComponent* Equipment = nullptr;
	UPROPERTY()
	UNSInventoryComponent* Inventory = nullptr;

protected:
	UFUNCTION()
	void OnRep_ActiveSlot();
};
