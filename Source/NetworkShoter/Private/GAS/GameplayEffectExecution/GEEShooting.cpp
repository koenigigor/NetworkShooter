// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectExecution/GEEShooting.h"

#include "AbilitySystemComponent.h"
#include "NSEquipment.h"
#include "Items/Weapon.h"

void UGEEShooting::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                          FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	//Get player weapon primary ability
	TSubclassOf<UGameplayAbility> PrimaryAbility;
	auto PlayerEquipment = ExecutionParams.GetSourceAbilitySystemComponent()->GetOwnerActor()->FindComponentByClass<UNSEquipment>();
	if (PlayerEquipment)
	{
		auto Weapon = PlayerEquipment->GetEquippedWeapon();
		PrimaryAbility =  Weapon->WeaponData->PrimaryAbility;
	}

	//Activate ability if valid
	if (IsValid(PrimaryAbility))
	{
		bool result = ExecutionParams.GetSourceAbilitySystemComponent()->TryActivateAbilityByClass(PrimaryAbility);
		if (!result)
		{
			UE_LOG(LogTemp, Warning, TEXT("PrimaryAbility not activated"))
		};
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimaryAbility not found"))
	}
}
