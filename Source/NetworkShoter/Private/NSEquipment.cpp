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
	const auto WeaponData = Weapon->WeaponData;

	Weapon -> SetOwner(GetOwner());
	Weapon -> SetInstigator(GetOwner()->GetInstigator());

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
			//first loop if weapon already in storage
			//second for find free spot
			bool bFreeSlot = false;
			for (auto& WeaponSlot : Weapons)
			{
				if (WeaponSlot == Weapon)
				{
					bFreeSlot=true;
					WeaponSlot = Weapon;
					break;
				}
			}
			
			if (!bFreeSlot)
			{
				for (auto& WeaponSlot : Weapons)
				{
					if (WeaponSlot == nullptr)
					{
						bFreeSlot=true;
						WeaponSlot = Weapon;
						break;
					}
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
	//check if dont have other weapon
	bool LastWeapon = true;
	for (int32 i = 0; i< MaxWeaponSlots; i++)
	{
		if (Weapons[i] != nullptr && Weapons[i] != EquippedWeapon)
		{
			LastWeapon = false;
			break;
		}
	}
	
	if (LastWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant drop last weapon"))
		return nullptr;
	}
	
	//Unequip weapon with drop it in world
	AWeapon* Weapon = UnequipWeapon(false);
	
	EquipNextWeapon();
	
	return Weapon;
}

bool UNSEquipment::EquipWeapon(int32 Slot)
{
	if (Slot == EquippedWeaponSlot) { return false; }
	if (!Weapons[Slot])	{ return false; };
	
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipWeapon(Slot);
		return false;
	}
	
	auto WeaponToEquip = Weapons[Slot];
	
	//Remove weapon if has already equipped
	UnequipWeapon(true);
	
	//attach weapon to owner
		//get mesh to attach //TODO interface for get component for 1st 3rd person if need
	USkeletalMeshComponent* MeshToAttach = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	
	FName Socket = "Gun";
	WeaponToEquip->AttachToComponent(MeshToAttach, FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
	
	//apply new weapon stats to player and register weapon ability
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

	EquippedWeaponSlot = Slot;
	EquippedWeapon = WeaponToEquip;

	//Unhide weapon from storage
	EquippedWeapon -> SetStatus(EWeaponStatus::Equipped);
		
	return true;
}

void UNSEquipment::EquipNextWeapon(bool Up)
{
	if (!GetWorld()) { return; } // loop runs on start editor?
	
	int32 NextSlot = EquippedWeaponSlot;
	
	//find and equip next weapon
	do
	{
		NextSlot = Up ? NextSlot + 1 : NextSlot - 1;
		
		if (NextSlot > MaxWeaponSlots) { NextSlot = 0; }
		if (NextSlot < 0) { NextSlot = MaxWeaponSlots; }
		
		if (Weapons.IsValidIndex(NextSlot) && Weapons[NextSlot])
		{
			EquipWeapon(NextSlot);
			return;
		}
	}
	while (NextSlot != EquippedWeaponSlot);
	
	UE_LOG(LogTemp, Warning, TEXT("Next weapon not found"))
	return;
}


TArray<AWeapon*> UNSEquipment::GetAllWeapons()
{
	return Weapons;
}

AWeapon* UNSEquipment::GetGrenade(int32 Slot, bool bWithRemove)
{
	if (!Grenades.IsValidIndex(Slot))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Grenade in slot %f"), Slot)
		return nullptr;
	}
	
	auto GrenadeData = Grenades[Slot].GrenadeData;
		
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
	
	AWeapon* SpawnedWeapon =GetOwner() -> GetWorld() -> SpawnActor<AWeapon>(SpawnLocation, SpawnRotation, SpawnParameters);
	SpawnedWeapon->SetupData(GrenadeData);
	
	return SpawnedWeapon;
}

AWeapon* UNSEquipment::GetSelectedGrenade(bool bWithRemove)
{
	return GetGrenade(SelectedGrenadeSlot, bWithRemove);
}


void UNSEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UNSEquipment, EquippedWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UNSEquipment, EquippedWeaponSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UNSEquipment, Weapons, COND_OwnerOnly);
}

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
		SpawnParameters.Instigator = GetOwner()->GetInstigator();
	
		AWeapon* Weapon = GetWorld() -> SpawnActor<AWeapon>(SpawnLocation, SpawnRotation, SpawnParameters);
		Weapon->SetupData(DefaultWeapon);

		//Add and equip weapon
		PickUpWeapon(Weapon);
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
		else
		{
			//delete weapon info from storage
			Weapons[EquippedWeaponSlot] -> SetStatus(EWeaponStatus::InWorld);
			Weapons[EquippedWeaponSlot] = nullptr;
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

void UNSEquipment::OnRep_Weapons()
{
	WeaponUpdated.Broadcast();
}

void UNSEquipment::ServerEquipWeapon_Implementation(int32 Slot)
{
	EquipWeapon(Slot);
}



