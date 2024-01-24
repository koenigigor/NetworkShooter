// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayEffectTypes.h"
#include "AbilityBPLibrary.generated.h"

UCLASS()
class NETWORKSHOTER_API UAbilityBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Manually sets the period on a specific effect */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecHandle SetPeriod(FGameplayEffectSpecHandle SpecHandle, float Period);

	UE_DEPRECATED(4.27, "use SetEffectInstigatorAndCauser")
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecHandle SetEffectCauser(const FGameplayEffectSpecHandle& SpecHandle, AActor* Causer);

	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecHandle SetEffectInstigatorAndCauser(const FGameplayEffectSpecHandle& SpecHandle, AActor* Instigator, AActor* Causer);
};
