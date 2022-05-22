// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EFragment_SpawnActors.h"

#include "Equipment/NSEquipmentInstance.h"
#include "GameFramework/Character.h"

void UEFragment_SpawnActors::OnEquip(UNSEquipmentInstance* Instance)
{
	Super::OnEquip(Instance);

	UE_LOG(LogTemp, Warning, TEXT("Fragment spawn actors called"))
	
	check(Instance && Instance->GetInstigator());
	
	USceneComponent* AttachTarget = Instance->GetInstigator()->GetRootComponent();
	if (auto Character = Cast<ACharacter>(Instance->GetInstigator()))
	{
		AttachTarget = Character->GetMesh(); //todo interface for fp/tp
	}

	for (const auto& ActorToSpawn : ActorsToSpawn)
	{
		//spawn actor
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = Instance->GetInstigator();
		SpawnParameters.Instigator = Cast<APawn>(Instance->GetInstigator());
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
		AActor* Spawned = GetWorld() -> SpawnActor<AActor>(ActorToSpawn.Actor, SpawnParameters);

		Spawned->SetActorEnableCollision(false); //todo i wanna keep collision

		Spawned->AttachToComponent(
			AttachTarget,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			ActorToSpawn.Socket);

		Instance->SpawnedActors.Add(Spawned);

		UE_LOG(LogTemp, Warning, TEXT("Actor spawn and attach %d"), IsValid(Spawned))
	} 
}

void UEFragment_SpawnActors::OnUnequip(UNSEquipmentInstance* Instance)
{
	Super::OnUnequip(Instance);

	for (auto& SpawnedActor : Instance->SpawnedActors)
	{
		SpawnedActor->Destroy();
	}
}
