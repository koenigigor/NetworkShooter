// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NSPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerStatisticUpdateDelegate);

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

	UPROPERTY(BlueprintAssignable)
	FPlayerStatisticUpdateDelegate PlayerStatisticUpdateDelegate;
	
	/** Add 1 kill in player statistic*/
	UFUNCTION()
	void AddKill();

	UFUNCTION()
	void AddAssist();

	UFUNCTION()
	void AddDeath();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerStatistic GetPlayerStatistic() { return PlayerStatistic; }

protected:
	UPROPERTY(ReplicatedUsing = "OnRep_PlayerStatistic")
	FPlayerStatistic PlayerStatistic;

	UFUNCTION()
	void OnRep_PlayerStatistic();
};
