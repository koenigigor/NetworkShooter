// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NSStructures.generated.h"

/**
 * Struct for store battle stat
 */
USTRUCT(BlueprintType)
struct FPlayerStatistic
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 KillCount = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 DeathCount = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 AssistCount = 0;

	FPlayerStatistic& operator+=(const FPlayerStatistic& Other);
};

/** Struct for keep info about damage */
USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	FDamageInfo(){};
	FDamageInfo(AActor* InInstigator, AActor* InCauser, AActor* InTarget, float InDamage);
	
	UPROPERTY(BlueprintReadOnly)
	AActor* Instigator = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* DamageCauser = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* Target = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	float Damage = 0.f;

	/** Time when damage was been applied */
	UPROPERTY(BlueprintReadOnly)
	float Time = 0.f;
};

/** Damage indo who send to client (for chat and kill feed) */
USTRUCT(BlueprintType)
struct FDamageInfoChat
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag MessageTag; //Chat.System.Damage / Chat.System.Kill / Chat.System.Heal
	
	UPROPERTY(BlueprintReadOnly)
	FString Instigator;
	
	UPROPERTY(BlueprintReadOnly)
	FString DamageCauser;
	
	UPROPERTY(BlueprintReadOnly)
	FString Target;
	
	UPROPERTY(BlueprintReadOnly)
	float Damage = 0.f;

	/** Time when damage was been applied */
	UPROPERTY(BlueprintReadOnly)
	float Time = 0.f;
	
	//todo weapon ref for get ui image, if is weapon
};