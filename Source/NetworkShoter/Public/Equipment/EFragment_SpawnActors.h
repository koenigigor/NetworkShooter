// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/NSEquipmentDefinition.h"
#include "EFragment_SpawnActors.generated.h"

USTRUCT(BlueprintType)
struct FEquipmentActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> Actor;

	UPROPERTY(EditDefaultsOnly)
	FName Socket;
};

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UEFragment_SpawnActors : public UNSEquipmentFragment
{
	GENERATED_BODY()

public:
	//spawn actor, attach socket, offset

	virtual void OnEquip(UNSEquipmentInstance* Instance) override;
	virtual void OnUnequip(UNSEquipmentInstance* Instance) override;

	UPROPERTY(EditDefaultsOnly)
	TArray<FEquipmentActor> ActorsToSpawn;
};
