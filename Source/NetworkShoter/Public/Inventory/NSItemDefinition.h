// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSItemDefinition.generated.h"

class UNSItemInstance;


/** Single fragment information about item */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract, Blueprintable)
class UNSInventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:
	virtual void OnInstanceCreated(UNSItemInstance* Instance) {};
};


/**
 * Item definition class
 * contain different fragments information about item
 */
UCLASS(Blueprintable, Const, Abstract)
class NETWORKSHOTER_API UNSItemDefinition : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	TSubclassOf<UNSItemInstance> InstanceType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UNSInventoryItemFragment>> Fragments;

public:
	UNSItemInstance* CreateInstance(UObject* Outer) const;

	TSubclassOf<UNSItemInstance> GetInstanceType() const;
	
	UNSInventoryItemFragment* FindFragmentByClass(TSubclassOf<UNSInventoryItemFragment> FragmentClass) const;
	
	template <class T>
	T* FindFragmentByClass() const
	{
		return (T*)FindFragmentByClass(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, DisplayName="FindFragmentByClass", Category="Inventory", meta=(DeterminesOutputType=FragmentClass))
	static const UNSInventoryItemFragment* FindFragmentByClass(const TSubclassOf<UNSItemDefinition> ItemDef, const TSubclassOf<UNSInventoryItemFragment> FragmentClass);
};
