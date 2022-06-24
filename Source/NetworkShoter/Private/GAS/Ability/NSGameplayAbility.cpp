// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/NSGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "Inventory/NSInventoryComponent.h"

void UNSGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TryActivateOnSpawn(ActorInfo, Spec);
}

void UNSGameplayAbility::TryActivateOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);
	
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && GetActivationPolicy() == EActivationPolicy::ActivateOnGrant)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

bool UNSGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   FGameplayTagContainer* OptionalRelevantTags) const
{
	const auto bParentCost = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);

	if (!bParentCost || ItemCost.Num() == 0) return bParentCost;
	
	const auto Inventory = GetOwningActorFromActorInfo()->FindComponentByClass<UNSInventoryComponent>();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory not find fot check ability %s cost"), *GetName())
		return false;
	}

	return Inventory->CheckItems(ItemCost);
}

void UNSGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	const auto Inventory = GetOwningActorFromActorInfo()->FindComponentByClass<UNSInventoryComponent>();
	TArray<FInventoryEntry> RemovedItems;
	Inventory->RemoveItems(ItemCost, RemovedItems, true, false);
}
