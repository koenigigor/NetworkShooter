// Fill out your copyright notice in the Description page of Project Settings.


#include "NSEquipment.h"

#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "NetworkShoter/Weapon.h"

// Sets default values for this component's properties
UNSEquipment::UNSEquipment()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Weapons.SetNum(MaxWeaponSlots);
}

bool UNSEquipment::PickUpWeapon(AWeapon* Weapon)
{
	//GetWeaponData
	auto WeaponData = Weapon->WeaponData;
	
	UE_LOG(LogTemp, Warning, TEXT("PickUpWeapon item name = %s"), *(WeaponData->Name.ToString()))

	Weapon -> SetOwner(GetOwner());
	Weapon -> SetInstigator(GetOwner()->GetInstigator()); //TODO or need cast

	//Switch by weapon type
	switch (WeaponData->Type)
	{
	case EWeaponType::Grenade:
		{
			bool bGrenadeFound = false;
			//found grenades in array
			for (auto& Grenade : Grenades)
			{
				if (Grenade.GrenadeData == WeaponData)
				{
					bGrenadeFound = true;
					Grenade.Count++;
				}
			}
			//if not found add new
			if (!bGrenadeFound)
			{
				Grenades.Add(FGrenadeCount(WeaponData, 1));
			}

			Weapon->Destroy();

			return true;
		}
		break;

	case EWeaponType::MeleeWeapon:
	case EWeaponType::RangeWeapon:
		{
			//add weapon in array
			bool bFreeSlot = false;
			for (auto& WeaponSlot : Weapons)
			{
				if (WeaponSlot == nullptr || WeaponSlot == Weapon)
				{
					bFreeSlot=true;
					WeaponSlot = Weapon;
					break;
				}
			}
			
			//if success freeze weapon
			if (bFreeSlot)
			{
				UE_LOG(LogTemp, Warning, TEXT("Weapon frized %s"), *Weapon->WeaponData->Name.ToString())
				//Weapon -> SetActorHiddenInGame(true); //not replicates?
				//Weapon -> SetActorEnableCollision(false);
				//Weapon -> bStoraged = true;
				Weapon -> SetStatus(EWeaponStatus::InStorage);
			}

			return true;
		}
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Cant recognise WeaponType, in weapon %s"), *(Weapon->GetName()))
		return false;
	}
}

bool UNSEquipment::EquipWeapon(int32 Slot)
{
	UE_LOG(LogTemp, Warning, TEXT("Start EquipWeapon"))

	if (!GetOwner()->HasAuthority())
	{
		ServerEquipWeapon(Slot);
		UE_LOG(LogTemp, Warning, TEXT("Equip weapon called not from server, redirect to server"))
		return false;
	}
	
	if (!Weapons[Slot])
	{
		return false;
	};
	auto WeaponToEquip = Weapons[Slot];
	
	//Remove weapon if has already equiped
	if (EquippedWeapon)
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		PickUpWeapon(EquippedWeapon);
		EquippedWeapon = nullptr;
	}
	
	//attach weapon to owner
		//get mesh to attach //TODO interface for get component for 1st 3rd person if need
	USkeletalMeshComponent* MeshToAttach = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	
	FName Socket = "Gun";
	WeaponToEquip->AttachToComponent(MeshToAttach, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Gun"));
	
	//apply new weapon stats to player
	if (auto IAbilitySystem = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		IAbilitySystem->GetAbilitySystemComponent()->InitStats(UWeaponAttributeSet::StaticClass(), WeaponToEquip->WeaponData->AttributeSet);
	};
	
	EquippedWeapon = WeaponToEquip;

	//Unhide weapon from storage
	UE_LOG(LogTemp, Warning, TEXT("Weapon unfrized %s"), *EquippedWeapon->WeaponData->Name.ToString())
	//EquippedWeapon -> SetActorHiddenInGame(false);
	//EquippedWeapon -> SetActorEnableCollision(false);
	//EquippedWeapon -> bStoraged = false;
	EquippedWeapon -> SetStatus(EWeaponStatus::Equiped);
		
	return true;
}

AWeapon* UNSEquipment::GetGrenade(int32 Slot, bool bWithRemove)
{
	if (Grenades.IsValidIndex(Slot))
	{
		//Get grenade from grenades array
		auto GrenadeData = Grenades[Slot].GrenadeData;
		//remove if need
		if (bWithRemove)
		{
			if (Grenades[Slot].Count <= 1)
			{
				Grenades.RemoveAt(Slot);
			}
			else
			{
				Grenades[Slot].Count --;
			}
		}
		
		//Spawn weapon actor
        FVector SpawnLocation(100, 100, 100);
        FRotator SpawnRotation(0,0,0);
        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Owner = GetOwner();
        SpawnParameters.Instigator = Cast<APawn>(GetOwner());
			UE_LOG(LogTemp, Warning, TEXT("Before spawn"))
        AWeapon* SpawnedWeapon =GetOwner() -> GetWorld() -> SpawnActor<AWeapon>(SpawnLocation, SpawnRotation, SpawnParameters);
			UE_LOG(LogTemp, Warning, TEXT("Afterspawn"))
        SpawnedWeapon->SetupData(GrenadeData);
			UE_LOG(LogTemp, Warning, TEXT("Aftersetup"))
		return SpawnedWeapon;
	}

	UE_LOG(LogTemp, Warning, TEXT("No Grenade in slot %f"), Slot)
	return nullptr;
}

AWeapon* UNSEquipment::GetSelectedGrenade(bool bWithRemove)
{
	return GetGrenade(SelectedGrenadeSlot, bWithRemove);
}

void UNSEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNSEquipment, EquippedWeapon);
}

// Called when the game starts
void UNSEquipment::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		CreateDefaultWeapon();
	}
}

void UNSEquipment::CreateDefaultWeapon()
{
	//Spawn weapon actor
	FVector SpawnLocation(0,0,-500);
	FRotator SpawnRotation(0,0,0);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = Cast<APawn>(GetOwner());
	
	AWeapon* Weapon = GetWorld() -> SpawnActor<AWeapon>(SpawnLocation, SpawnRotation, SpawnParameters);
	Weapon->SetupData(DefaultWeapon);

	//Add weapon in storage;
	PickUpWeapon(Weapon);

	//equip weapon
	EquipWeapon(0);
}

void UNSEquipment::ServerEquipWeapon_Implementation(int32 Slot)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerEquipWeapon_Implementation called"))
	EquipWeapon(Slot);
}



