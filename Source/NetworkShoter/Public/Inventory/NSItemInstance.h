// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAttributeContainer.h"
#include "NSItemInstance.generated.h"

class UNSInventoryItemFragment;
class UNSItemDefinition;

/**
 * Base class of item instance
 * contain array item attributes (like reliability, ammo in weapon, etc)
 */
UCLASS(BlueprintType) //for create in bp from create object
class NETWORKSHOTER_API UNSItemInstance : public UObject
{
	GENERATED_BODY()

	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//todo server redirections
public:
	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagValue(FGameplayTag Tag, float Value);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagValue(FGameplayTag Tag, float Value);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Inventory)
	float GetStatTagStackValue(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasAttributeTag(FGameplayTag Tag) const;

private:
	UPROPERTY(Replicated)
	FGameplayTagAttributeContainer ItemAttributes;
	

public:
	TSubclassOf<UNSItemDefinition> GetItemDefinition() const { return ItemDefinition; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass)) //todo why false
	UNSInventoryItemFragment* FindFragmentByClass(TSubclassOf<UNSInventoryItemFragment> FragmentClass) const;

	template <class T>
	const T* FindFragmentByClass()
	{
		return Cast<T>(FindFragmentByClass(T::StaticClass()));
	}

	/** Wrap MarkAsGarbage, because not sure it is true way? */
	void DestroyObject();
	
public:
	/** Initialise item, 1 time */
	UFUNCTION(BlueprintCallable)
	void SetItemDef(TSubclassOf<UNSItemDefinition> Definition);

private:
	UPROPERTY(Replicated)
	TSubclassOf<UNSItemDefinition> ItemDefinition;
};

