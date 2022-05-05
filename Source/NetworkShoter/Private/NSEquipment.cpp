// Fill out your copyright notice in the Description page of Project Settings.


#include "NSEquipment.h"

#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "Items/PlaceableWeapon.h"
#include "Items/Weapon.h"


UNSEquipment::UNSEquipment()
{
	PrimaryComponentTick.bCanEverTick = false;

	//Prepare weapon array
	Weapons.SetNum(MaxWeaponSlots);
}

void UNSEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UNSEquipment, EquippedWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UNSEquipment, EquippedWeaponSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UNSEquipment, StoredSpecialClass, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION_NOTIFY(UNSEquipment, Weapons, COND_None, REPNOTIFY_Always);
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


//~==============================================================================================
// Pickup 

bool UNSEquipment::PickUpWeapon(AWeapon* Weapon)
{
	const auto WeaponData = Weapon->WeaponData;

	Weapon -> SetOwner(GetOwner());
	Weapon -> SetInstigator(GetOwner()->GetInstigator());

	//Switch by weapon type
	bool bSuccess = false;
	switch (WeaponData->Type)
	{
	case EWeaponType::Grenade:
		bSuccess = AddGrenade(Weapon->GetClass());
		if (bSuccess)
		{
			Weapon->Destroy();
		}
		break;

	case EWeaponType::MeleeWeapon:
	case EWeaponType::RangeWeapon:
		bSuccess = AddWeapon(Weapon);
		UE_LOG(LogTemp, Warning, TEXT("Weapon added: %d"), bSuccess)
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("Cant recognise WeaponType, in weapon %s"), *(Weapon->GetName()))
	}
	
	return bSuccess;
}

bool UNSEquipment::AddWeapon(AWeapon* Weapon)
{
	//if already in storage
	bool bInStorage = false;
	for (auto& WeaponSlot : Weapons)
	{
		if (WeaponSlot == Weapon)
		{
			bInStorage=true;
			WeaponSlot = Weapon;
			break;
		}
	}

	//search free slot
	if (!bInStorage)
	{
		for (auto& WeaponSlot : Weapons)
		{
			if (WeaponSlot == nullptr)
			{
				bInStorage=true;
				WeaponSlot = Weapon;
				break;
			}
		}
	}
			
	//if success freeze weapon
	if (bInStorage)
	{
		Weapon -> SetOwner(GetOwner());
		Weapon -> SetInstigator(GetOwner()->GetInstigator());
		Weapon -> SetStatus(EWeaponStatus::InStorage);
	}

	return bInStorage;
}

bool UNSEquipment::AddGrenade(TSubclassOf<AWeapon> PickedGrenade, int32 Count)
{
	if (!PickedGrenade) return false;
	
	bool bGrenadeFound = false;
	
	//found grenades in array
	for (auto& Grenade : Grenades)
	{
		if (Grenade.Grenade == PickedGrenade)
		{
			bGrenadeFound = true;
			Grenade.Count += Count;
		}
	}
	
	//if not found add new
	if (!bGrenadeFound)
	{
		Grenades.Add(FGrenadeCount(PickedGrenade, Count));
	}
	
	return true;
}

void UNSEquipment::AddSpecial(TSubclassOf<APlaceableWeapon> PickedSpecial)
{
	StoredSpecialClass = PickedSpecial;
}


//~==============================================================================================
// MainWeapon 

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

void UNSEquipment::EquipWeapon_Implementation(int32 Slot)
{
	if (Slot == EquippedWeaponSlot) { return; }
	if (!Weapons[Slot])	{ return; };
	
	auto WeaponToEquip = Weapons[Slot];
	
	//Remove previous weapon
	UnequipWeapon(true);
	
	//attach weapon to owner
	USkeletalMeshComponent* MeshToAttach = GetOwner()->FindComponentByClass<USkeletalMeshComponent>(); //todo FP/TP mesh
	FName Socket = "Gun";
	WeaponToEquip->AttachToComponent(MeshToAttach, FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
	
	RegisterWeaponAbilities(WeaponToEquip);

	EquippedWeaponSlot = Slot;
	EquippedWeapon = WeaponToEquip;

	//Unhide weapon from storage
	EquippedWeapon -> SetStatus(EWeaponStatus::Equipped);

	SlotSelected.Broadcast();
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

AWeapon* UNSEquipment::UnequipWeapon(bool bAddInStorage)
{
	
	if (!EquippedWeapon) { return nullptr; };
	
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
	UnregisterWeaponAbilities(EquippedWeapon);

	auto TempWeapon = EquippedWeapon;
	EquippedWeapon = nullptr;
	return TempWeapon;
}

void UNSEquipment::RegisterWeaponAbilities(AWeapon* Weapon)
{
	auto IAbilitySystem = Cast<IAbilitySystemInterface>(GetOwner());
	if (!ensure(IAbilitySystem)) { return; }
	auto AbilitySystem = IAbilitySystem->GetAbilitySystemComponent();

	//bad way, or, maybe can change montage node to owner
	//AbilitySystem->SetAvatarActor_Direct(Weapon);

	
	//Copy weapon attribute
	TArray<UAttributeSet*> AllAttributes = AbilitySystem->GetSpawnedAttributes();
	for (const auto& Attribute : AllAttributes)
	{
		if (Attribute && Attribute->IsA(UWeaponAttributeSet::StaticClass()))
		{
			auto WeaponAttributeSet = Cast<UWeaponAttributeSet>(Attribute);
			WeaponAttributeSet->CopyFrom(Weapon->WeaponAttributeSet);
		}
	} 

	//GiveAbility
	if (IsValid(Weapon->WeaponData->PrimaryAbility))
	{
		auto AbilitySpec = FGameplayAbilitySpec(Weapon->WeaponData->PrimaryAbility);
		AbilitySpec.SourceObject = Weapon;
		AbilitySystem->GiveAbility(AbilitySpec);
	}
	if (IsValid(Weapon->WeaponData->SecondaryAbility))
	{
		auto AbilitySpec = FGameplayAbilitySpec(Weapon->WeaponData->SecondaryAbility);
		AbilitySpec.SourceObject = Weapon;
		AbilitySystem->GiveAbility(AbilitySpec);
	}
	if (IsValid(Weapon->WeaponData->Throw))
	{
		auto AbilitySpec = FGameplayAbilitySpec(Weapon->WeaponData->Throw);
		AbilitySpec.SourceObject = Weapon;
		AbilitySystem->GiveAbility(AbilitySpec);
	}
}

void UNSEquipment::UnregisterWeaponAbilities(AWeapon* Weapon)
{
	if (!GetOwner()->HasAuthority()) return;

	auto IAbilitySystem = Cast<IAbilitySystemInterface>(GetOwner());
	if (!ensure(IAbilitySystem)) { return; }
	auto AbilitySystem = IAbilitySystem->GetAbilitySystemComponent();
	
	
	//Save weapon attributes
	TArray<UAttributeSet*> AllAttributes = AbilitySystem->GetSpawnedAttributes();
	for (const auto& Attribute : AllAttributes)
	{
		if (Attribute && Attribute->IsA(UWeaponAttributeSet::StaticClass()))
		{
			auto WeaponAttributeSet = Cast<UWeaponAttributeSet>(Attribute);
			EquippedWeapon->WeaponAttributeSet->CopyFrom(WeaponAttributeSet);
		}
	} 	

	//ClearAbility
	if (IsValid(EquippedWeapon->WeaponData->PrimaryAbility))
		AbilitySystem->ClearAbility(AbilitySystem->FindAbilitySpecFromClass(EquippedWeapon->WeaponData->PrimaryAbility)->Handle);
	if (IsValid(EquippedWeapon->WeaponData->SecondaryAbility))
		AbilitySystem->ClearAbility(AbilitySystem->FindAbilitySpecFromClass(EquippedWeapon->WeaponData->SecondaryAbility)->Handle);
	if (IsValid(EquippedWeapon->WeaponData->Throw))
		AbilitySystem->ClearAbility(AbilitySystem->FindAbilitySpecFromClass(EquippedWeapon->WeaponData->Throw)->Handle);
}


//~==============================================================================================
// Special (spawnable) items

TSubclassOf<APlaceableWeapon> UNSEquipment::GetStoredSpecial()
{
	return StoredSpecialClass;
}

bool UNSEquipment::RemoveStoredSpecial(TSubclassOf<APlaceableWeapon> SpecialToRemove)
{
	if (StoredSpecialClass && StoredSpecialClass == SpecialToRemove)
	{
		StoredSpecialClass = nullptr;
		return true;
	}
	return false;
}


//~==============================================================================================
// Grenade

AWeapon* UNSEquipment::GetGrenade(int32 Slot, bool bWithRemove)
{
	if (!Grenades.IsValidIndex(Slot))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Grenade in slot %d"), Slot)
		return nullptr;
	}
	
	auto GrenadeClass = Grenades[Slot].Grenade;
		
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
	
	AWeapon* SpawnedWeapon =GetOwner() -> GetWorld() -> SpawnActor<AWeapon>(GrenadeClass, SpawnLocation, SpawnRotation, SpawnParameters);
	
	return SpawnedWeapon;
}

AWeapon* UNSEquipment::GetSelectedGrenade(bool bWithRemove)
{
	return GetGrenade(SelectedGrenadeSlot, bWithRemove);
}



void UNSEquipment::OnRep_SelectWeapon()
{
	SlotSelected.Broadcast();
}

void UNSEquipment::OnRep_Weapons()
{
	WeaponUpdated.Broadcast();
}
