// Fill out your copyright notice in the Description page of Project Settings.


#include "NSEquipment.h"

#include "NetworkShoter/Weapon.h"

// Sets default values for this component's properties
UNSEquipment::UNSEquipment()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UNSEquipment::PickUpWeapon(AWeapon* Weapon)
{
	//GetWeaponData
	auto WeaponData = Weapon->WeaponData;
	
	UE_LOG(LogTemp, Warning, TEXT("PickUpWeapon item name = %s"), *(WeaponData->Name.ToString()))


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
		}
		break;

	case EWeaponType::MeleeWeapon:
	case EWeaponType::RangeWeapon:

		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Cant recognise WeaponType, in weapon %s"), *(Weapon->GetName()))			
	}
	
	//is grenade

	//is weapon

	return true;
}

bool UNSEquipment::EquipWeapon(int32 Slot)
{
	//Remove weapon if has already equiped

	//attach weapon to owner

	//apply new weapon stats to player

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


// Called when the game starts
void UNSEquipment::BeginPlay()
{
	Super::BeginPlay();

	CreateDefaultWeapon();
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

	//Add weapon in array;
	Weapons.Empty();
	Weapons.Add(Weapon);

	//equip weapon
	EquipWeapon(0);
}



