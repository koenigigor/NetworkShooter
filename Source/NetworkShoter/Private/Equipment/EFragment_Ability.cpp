// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EFragment_Ability.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Equipment/NSEquipmentInstance.h"
#include "GAS/Ability/NSGameplayAbility_FromEquipment.h"

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
		Instance->GrantedAbilities.Add(ASC->GiveAbility(AbilitySpec));
	}
}

void UEFragment_Ability::OnUnequip(UNSEquipmentInstance* Instance)
{
	Super::OnUnequip(Instance);

	const auto Owner = Instance->GetInstigator();
	ensure(Owner);
	
	UAbilitySystemComponent* ASC = GetOwnerASC(Owner);
	if (!ASC) return;

	for (const auto& AbilitySpecHandle : Instance->GrantedAbilities)
	{
		bool bForceCancelAbility = false;
		if (const auto Spec = ASC->FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			if (const auto Ability = Cast<UNSGameplayAbility_FromEquipment>(Spec->Ability))
			{
				bForceCancelAbility = Ability->IsForceCancel();
			}
		}
		
		if (bForceCancelAbility)
		{
			ASC->ClearAbility(AbilitySpecHandle);
		}
		else
		{
			ASC->SetRemoveAbilityOnEnd(AbilitySpecHandle);
		}
	} 
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
