// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/NSEquipmentInstance.h"

#include "Net/UnrealNetwork.h"


UWorld* UNSEquipmentInstance::GetWorld() const
{
	if (GetOuter()) return GetOuter()->GetWorld();
	return nullptr;
}

void UNSEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}
