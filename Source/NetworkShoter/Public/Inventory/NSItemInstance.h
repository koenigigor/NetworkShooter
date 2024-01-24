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
 * Instance constructs in Definition
 */
UCLASS(BlueprintType)
class NETWORKSHOTER_API UNSItemInstance : public UObject
{
	GENERATED_BODY()

	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory, meta=(GameplayTagFilter="ItemAttribute"))
	void AddStatTagValue(FGameplayTag Tag, float Value);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory, meta=(GameplayTagFilter="ItemAttribute"))
	void RemoveStatTagValue(FGameplayTag Tag, float Value);

	UFUNCTION(BlueprintCallable, Category=Inventory, meta=(GameplayTagFilter="ItemAttribute"))
	float GetStatTagStackValue(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Inventory, meta=(GameplayTagFilter="ItemAttribute"))
	bool HasAttributeTag(FGameplayTag Tag) const;
	
	FItemAttributeChange& GetItemAttributeValueChangeDelegate(FGameplayTag Attribute);

	/** If item in storage, remove it, if not in storage just destroy instance */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool ConsumeItem(int32 Count = 1);
	
protected:
	UPROPERTY(Replicated)
	FGameplayTagAttributeContainer ItemAttributes;
	

public:
	UFUNCTION(BlueprintPure)
	TSubclassOf<UNSItemDefinition> GetItemDefinition() const { return ItemDefinition; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass)) //todo why false
	UNSInventoryItemFragment* FindFragmentByClass(TSubclassOf<UNSInventoryItemFragment> FragmentClass) const;

	template <class T>
	const T* FindFragmentByClass()
	{
		return Cast<T>(FindFragmentByClass(T::StaticClass()));
	}

protected:
	friend UNSItemDefinition;
	
	UPROPERTY(Replicated)
	TSubclassOf<UNSItemDefinition> ItemDefinition;
};

