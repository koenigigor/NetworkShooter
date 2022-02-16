// Fill out your copyright notice in the Description page of Project Settings.


#include "GEEShooting.h"

#include "AbilitySystemComponent.h"

void UGEEShooting::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                          FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	FGameplayTagContainer TagContainer;

	TagContainer.AddTag(FGameplayTag::RequestGameplayTag("Skill.Weapon.Primary"));

	bool result = ExecutionParams.GetSourceAbilitySystemComponent()->TryActivateAbilitiesByTag(TagContainer);
	if (!result)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill.Weapon.Primary, not found"))
	};
}
