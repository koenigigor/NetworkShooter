// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "Abilities/GameplayAbility.h"
#include "NSStructures.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MESSAGE_SYSTEM_DAMAGE);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MESSAGE_SYSTEM_HEAL);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MESSAGE_SYSTEM_DEATH);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CHAT_GENERAL);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CHAT_GROUP);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_CHAT_LOCATION);

class UNSItemDefinition;
class UInputAction;

/** Initial Info for Item Slot Widget, used for construct Item Slot*/
USTRUCT(BlueprintType)
struct FSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UNSItemDefinition> ItemDef = nullptr;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Count = 0;

	/** Set if slot can be used by IA */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* Activation = nullptr;

	/** Empty frame if slot not valid */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* EmptyFrame = nullptr;
};

/** Struct for store battle stat */
USTRUCT(BlueprintType)
struct FPlayerStatistic
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
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

	FDamageInfo();
	FDamageInfo(APlayerState* InInstigatorState, AActor* InCauser, AActor* InTarget, float InDamage, const UGameplayAbility* InSourceAbilityCDO = nullptr);

	/** return message based this info,
	 *	try call IDamageDescriptionInterface
	 *	on source Ability, Causer, or, make default message */
	FString GetMessage() const;

	bool IsValid() const;
	
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
