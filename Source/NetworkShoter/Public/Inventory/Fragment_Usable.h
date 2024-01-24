// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inventory/NSItemDefinition.h"
#include "Fragment_Usable.generated.h"

class UAbilitySystemComponent;
class UNSGameplayAbility;
class UGameplayAbility;

/**
 * Grant all item abilities to player, on player start, for use LocalPredicted Ability instantly.
 * in future item ability can be add on item add in inventory, like in ActionRPG example.
 * Or make ability server side, and grant on use, but then, i think, we will be have delay between press and activation
 */
UCLASS()
class UItemAbilitiesData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> ItemAbilities;
};

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UFragment_Usable : public UNSInventoryItemFragment
{
	GENERATED_BODY()
public:
	/** Gameplay tag for activate ability by gameplay event */
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ActivationTag;

	/** return tag for gameplay ability activation */
	UFUNCTION(BlueprintCallable, Category = "Item", meta = (ExpandBoolAsExecs="bFound"))
	static FGameplayTag GetAbilityActivationTag(UNSItemInstance* Instance, bool& bFound);

	/** Item ability, for Grant it on add item in inventory */ //todo
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UNSGameplayAbility> AbilityToGrant;

	
	//depr
	//UFUNCTION(BlueprintCallable)
	//static void UseItem(UAbilitySystemComponent* ASC, UNSItemInstance* Instance);
};
