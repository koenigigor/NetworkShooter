// Fill out your copyright notice in the Description page of Project Settings.


#include "NSEquipment.h"

#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "NetworkShoter/Weapon.h"


UNSEquipment::UNSEquipment()
{
	PrimaryComponentTick.bCanEverTick = false;

	//Prepare weapon array
	Weapons.SetNum(MaxWeaponSlots);
}

bool UNSEquipment::PickUpWeapon(AWeapon* Weapon)
{
	//GetWeaponData
	auto WeaponData = Weapon->WeaponData;

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

AWeapon* UNSEquipment::DropCurrentWeapon()
{
	//if dont have other weapon in storage return
	//equip next weapon
	int32 OtherWeaponSlot = -1;
	for (int32 i = 0; i< MaxWeaponSlots; i++)
	{
		if (Weapons[i] != nullptr && Weapons[i] != EquippedWeapon)
		{
			OtherWeaponSlot = i;
			break;
		}
	}
	if (OtherWeaponSlot == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant drop weapon if you hawe only 1"))
		return nullptr;
	}
	
	//Unequip weapon
	AWeapon* Weapon = UnequipWeapon(false);
	
	//delete weapon info from storage
	for (auto& StoredWeapon : Weapons)
	{
		if (StoredWeapon == Weapon)
		{
			StoredWeapon = nullptr;
			break;
		}
	}
	
	//set visibility
	Weapon->SetStatus(EWeaponStatus::InWorld);

	EquipWeapon(OtherWeaponSlot);
	
	return Weapon;
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
	
	//Remove weapon if has already equipped
	UnequipWeapon(true);
	
	//attach weapon to owner
		//get mesh to attach //TODO interface for get component for 1st 3rd person if need
	USkeletalMeshComponent* MeshToAttach = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	
	FName Socket = "Gun";
	WeaponToEquip->AttachToComponent(MeshToAttach, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Gun"));
	
	//apply new weapon stats to player and register weapon ablilityes
	if (auto IAbilitySystem = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		auto AbilitySystem = IAbilitySystem->GetAbilitySystemComponent();
		
		AbilitySystem->InitStats(UWeaponAttributeSet::StaticClass(), WeaponToEquip->WeaponData->AttributeSet);

		if (IsValid(WeaponToEquip->WeaponData->PrimaryAbility))
			AbilitySystem->GiveAbility(FGameplayAbilitySpec(WeaponToEquip->WeaponData->PrimaryAbility));
		if (IsValid(WeaponToEquip->WeaponData->SecondaryAbility))
			AbilitySystem->GiveAbility(FGameplayAbilitySpec(WeaponToEquip->WeaponData->SecondaryAbility));
		if (IsValid(WeaponToEquip->WeaponData->Throw))
			AbilitySystem->GiveAbility(FGameplayAbilitySpec(WeaponToEquip->WeaponData->Throw));
	};
	
	EquippedWeapon = WeaponToEquip;

	//Unhide weapon from storage
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

	//Create default weapon
	if (GetOwner()->HasAuthority())
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
}

AWeapon* UNSEquipment::UnequipWeapon(bool bAddInStorage)
{
	if (EquippedWeapon)
	{
		//detach
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (bAddInStorage)
		{
			PickUpWeapon(EquippedWeapon);
		}
		
		//remove weapon abilities
		if (auto IAbilitySystem = Cast<IAbilitySystemInterface>(GetOwner()))
		{
			auto AbilitySystem = IAbilitySystem->GetAbilitySystemComponent();
			
			if (IsValid(EquippedWeapon->WeaponData->PrimaryAbility))
				AbilitySystem->ClearAbility(AbilitySystem->FindAbilitySpecFromClass(EquippedWeapon->WeaponData->PrimaryAbility)->Handle);
			if (IsValid(EquippedWeapon->WeaponData->SecondaryAbility))
				AbilitySystem->ClearAbility(AbilitySystem->FindAbilitySpecFromClass(EquippedWeapon->WeaponData->SecondaryAbility)->Handle);
			if (IsValid(EquippedWeapon->WeaponData->Throw))
				AbilitySystem->ClearAbility(AbilitySystem->FindAbilitySpecFromClass(EquippedWeapon->WeaponData->Throw)->Handle);
		}

		auto TempWeapon = EquippedWeapon;
		EquippedWeapon = nullptr;
		return TempWeapon;
	}
	return nullptr;
}

void UNSEquipment::ServerEquipWeapon_Implementation(int32 Slot)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerEquipWeapon_Implementation called"))
	EquipWeapon(Slot);
}



