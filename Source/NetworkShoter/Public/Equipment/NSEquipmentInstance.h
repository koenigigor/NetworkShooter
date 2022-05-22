// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSEquipmentInstance.generated.h"

class UNSItemInstance;

/**
 * Support equipment item object.
 * Unlike lyra, i not separate item and equipment item classes
 * this object contains only extended info about equipment
 * like spawned actors, or etc.
 */
UCLASS(BlueprintType, Blueprintable)
class NETWORKSHOTER_API UNSEquipmentInstance : public UObject
{
	GENERATED_BODY()
public:
	//void Equip(); //where init definition 
	//void Unequip();
	
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category=Equipment)
	AActor* GetInstigator() const { return Instigator; }

	void SetInstigator(AActor* InInstigator) { Instigator = InInstigator; }

	UFUNCTION()
	void OnRep_Instigator();
	

	/** Array spawned actors, if we spawn actors on equip */
	UPROPERTY(Replicated)
	TArray<AActor*> SpawnedActors;

	UPROPERTY(ReplicatedUsing="OnRep_Instigator")
	AActor* Instigator = nullptr;
};

//todo fragment on equip apply item attributes to owner