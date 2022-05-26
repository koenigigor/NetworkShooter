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
UCLASS(BlueprintType)
class NETWORKSHOTER_API UNSItemInstance : public UObject
{
	GENERATED_BODY()

	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagValue(FGameplayTag Tag, float Value);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagValue(FGameplayTag Tag, float Value);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	float GetStatTagStackValue(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasAttributeTag(FGameplayTag Tag) const;

protected:
	UPROPERTY(Replicated)
	FGameplayTagAttributeContainer ItemAttributes;

	
public:
   	/** Initialise item, 1 time */
   	UFUNCTION(BlueprintCallable)
   	void InitDefinition(TSubclassOf<UNSItemDefinition> Definition);

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
	UPROPERTY(Replicated)
	TSubclassOf<UNSItemDefinition> ItemDefinition;
};

