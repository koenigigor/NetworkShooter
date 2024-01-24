// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "NSEquipmentInstance.generated.h"

class UNSItemInstance;

/**
 * EquipmentInstance contains  info about equipped equipment
 * like spawned actors, granted abilities, or etc.
 */
UCLASS(BlueprintType, Blueprintable)
class NETWORKSHOTER_API UNSEquipmentInstance : public UObject
{
	GENERATED_BODY()
public:	
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category=Equipment)
	AActor* GetInstigator() const { return Instigator; }

	void SetInstigator(AActor* InInstigator) { Instigator = InInstigator; }

	UFUNCTION()
	void OnRep_Instigator() {}; //make virtual in future
	

	/** Array spawned actors, if we spawn actors on equip */
	UPROPERTY(Replicated)
	TArray<AActor*> SpawnedActors;

	TArray<FGameplayAbilitySpecHandle> GrantedAbilities;

	UPROPERTY(ReplicatedUsing="OnRep_Instigator")
	AActor* Instigator = nullptr;

	/** Item who response for create this weapon */
	UPROPERTY(BlueprintReadOnly)
	UNSItemInstance* SourceItem = nullptr;

	/** if has EFragment_Attributes keep this effect who response for add equipment attributes */
	FActiveGameplayEffectHandle ApplyAttributesEffectHandle;
};