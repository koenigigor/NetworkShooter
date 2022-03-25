// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NSPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerStatisticUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDeadDelegate); 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterRespawnDelegate);

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
};

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	//~==============================================================================================
	// Delegates
	UPROPERTY(BlueprintAssignable)
	FPlayerStatisticUpdateDelegate PlayerStatisticUpdateDelegate;

	//called from character, when he die
	UPROPERTY(BlueprintAssignable)
	FCharacterDeadDelegate CharacterDeadDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FCharacterRespawnDelegate CharacterRespawnDelegate;

	
	/** [All sides] */
	UFUNCTION()
	void OnCharacterDeath();

	/** [Multicast] GameMode call this when character respawn, for send respawn delegate to all client*/
	UFUNCTION(NetMulticast, Reliable)
	void RespawnHandle();

	bool IsLife();
	
	bool bDeath = true;

	//~==============================================================================================
	// Team

	/** Team index, setup on login */
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TeamIndex;
	
	//~==============================================================================================
	// PlayerStatistic
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerStatistic GetPlayerStatistic() { return PlayerStatistic; }
	
	/** Increment kill statistic */
	UFUNCTION()
	void AddKill();

	/** Increment assist statistic */
	UFUNCTION()
	void AddAssist();

	/** Increment death statistic */
	UFUNCTION()
	void AddDeath();
protected:
	UFUNCTION()
	void OnRep_PlayerStatistic();
	
	UPROPERTY(ReplicatedUsing = "OnRep_PlayerStatistic")
	FPlayerStatistic PlayerStatistic;
};
