// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Weapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"


AWeapon::AWeapon()
{
	bReplicates = true;

	//create component
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("Weapon");
	WeaponMesh->SetIsReplicated(true);
	RootComponent = WeaponMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetAutoActivate(false);
}

void AWeapon::PostInitProperties()
{
	Super::PostInitProperties();

	if (WeaponData)
	{
		SetupData(WeaponData);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeapon, Status);
	DOREPLIFETIME(AWeapon, WeaponData);
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

	
	//init attribute
	if (WeaponData->AttributeSet)
	{
		if (!WeaponAttributeSet)
		{ WeaponAttributeSet = NewObject<UWeaponAttributeSet>(this, UWeaponAttributeSet::StaticClass()); }
		
		WeaponAttributeSet->InitFromMetaDataTable(WeaponData->AttributeSet);
	}
	
}

void AWeapon::LaunchAsProjectile(FVector Velocity)
{
	ProjectileMovement->Velocity = Velocity;
	ProjectileMovement->Activate();
}

void AWeapon::SetStatus(EWeaponStatus NewStatus)
{
	//place for validation, server changes etc
	Status = NewStatus;
	OnRep_Status();
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