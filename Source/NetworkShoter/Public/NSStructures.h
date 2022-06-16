// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
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
	FDamageInfo(APlayerState* InInstigatorState, AActor* InCauser, AActor* InTarget, float InDamage, const UGameplayAbility* InSourceAbilityCDO = nullptr);

	/** return message based this info, see IDamageDescriptionInterface */
	FString GetMessage() const;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag Tag;
	
	UPROPERTY(BlueprintReadOnly)
	APlayerState* InstigatorState = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* DamageCauser = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	AActor* Target = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	float Damage = 0.f;

	UPROPERTY()
	const UGameplayAbility* SourceAbilityCDO = nullptr;
};

namespace NSTag
{
	namespace System
	{
		inline FGameplayTag Damage() { return FGameplayTag::RequestGameplayTag("Message.System.Damage"); }
		inline FGameplayTag Heal() { return FGameplayTag::RequestGameplayTag("Message.System.Heal"); }
        inline FGameplayTag Death() { return FGameplayTag::RequestGameplayTag("Message.System.Death"); }
	}

	namespace Chat
	{
		inline FGameplayTag Damage() { return FGameplayTag::RequestGameplayTag("Chat.System.Damage"); }
		inline FGameplayTag Heal() { return FGameplayTag::RequestGameplayTag("Chat.System.Heal"); }
		inline FGameplayTag Death() { return FGameplayTag::RequestGameplayTag("Chat.System.Death"); }
	}
	
}
