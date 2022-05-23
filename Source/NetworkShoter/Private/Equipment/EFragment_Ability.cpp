// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EFragment_Ability.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Equipment/NSEquipmentInstance.h"
#include "Equipment/NSGameplayAbility_FromEquipment.h"

void UEFragment_Ability::OnEquip(UNSEquipmentInstance* Instance)
{
	Super::OnEquip(Instance);

	const auto Owner = Instance->GetInstigator();
	ensure(Owner);

	UAbilitySystemComponent* ASC = GetOwnerASC(Owner);
	if (!ASC) return;

	for (const auto& Ability : AbilitiesToGrant)
	{
		//add ability
		auto AbilitySpec = FGameplayAbilitySpec(Ability);
		AbilitySpec.SourceObject = Instance;
		ASC->GiveAbility(AbilitySpec);
	}
}

void UEFragment_Ability::OnUnequip(UNSEquipmentInstance* Instance)
{
	Super::OnUnequip(Instance);
}

UAbilitySystemComponent* UEFragment_Ability::GetOwnerASC(AActor* Owner) const
{
	UAbilitySystemComponent* ASC = nullptr;
	if (const auto AbilityInterface = Cast<IAbilitySystemInterface>(Owner))
	{
		ASC = AbilityInterface->GetAbilitySystemComponent();
	}
	if (!ASC)
	{
		UE_LOG(LogEFragment, Display, TEXT("Owners ASC not found"));
	}

	return ASC;
}
