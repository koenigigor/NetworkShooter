// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayEffectTypes.h"
#include "MyGameplayEffectSpec.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API UMyGameplayEffectSpec : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Manually sets the period on a specific effect */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecHandle SetPeriod(FGameplayEffectSpecHandle SpecHandle, float Period);

	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecHandle SetEffectCauser(FGameplayEffectSpecHandle SpecHandle, AActor* Causer);
};
