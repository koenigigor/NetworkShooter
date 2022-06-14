// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayEffectTypes.h"
#include "MyGameplayEffectSpec.generated.h"

struct FGameplayEffectCustomExecutionParameters;
/**
 * todo rename MyGASSupportLib
 */
UCLASS()
class NETWORKSHOTER_API UMyGameplayEffectSpec : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Manually sets the period on a specific effect */
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecHandle SetPeriod(FGameplayEffectSpecHandle SpecHandle, float Period);

	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	static FGameplayEffectSpecHandle SetEffectCauser(const FGameplayEffectSpecHandle& SpecHandle, AActor* Causer);
};
