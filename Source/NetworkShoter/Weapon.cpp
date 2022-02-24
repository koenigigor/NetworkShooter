// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"


AWeapon::AWeapon()
{
	//PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//create component
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("Weapon");
	WeaponMesh->SetIsReplicated(true);
	RootComponent = WeaponMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetAutoActivate(false);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeapon, Status);
}

void AWeapon::SetupData(UWeaponData* Data)
{
	if (!Data)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon data not valid in weapon %s"), *GetName())
		return;
	}
	
	WeaponData = Data;
	
	//set mesh
	WeaponMesh->SetStaticMesh( Data->Mesh);
}

void AWeapon::LaunchAsProjectile(FVector Velocity)
{
	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->Activate();
	//WeaponMesh->SetNotifyRigidBodyCollision(true); //activate hit events
//	WeaponMesh->SetSimulatePhysics(true);
}

void AWeapon::OnRep_Status()
{
	switch (Status)
	{
	case EWeaponStatus::Equipped:
		SetActorEnableCollision(false);
		SetActorHiddenInGame(false);
		break;

	case EWeaponStatus::InStorage:
		SetActorEnableCollision(false);
		SetActorHiddenInGame(true);
		break;
		
	case EWeaponStatus::InWorld:
		SetActorEnableCollision(true);
		SetActorHiddenInGame(false);
		break;
	}
}

void AWeapon::SetStatus(EWeaponStatus NewStatus)
{
	//place for validation, server changes etc
	Status = NewStatus;
	OnRep_Status();
}
