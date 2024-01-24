// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerState.h"
#include "NSStructures.h"
#include "TeamAttitude.h"
#include "NSPlayerState.generated.h"

class UGameplayAbility;
class UNSAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerStatisticUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDeadDelegate, APawn*, DeadPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterRespawnDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeTeamDelegate, EGameTeam, NewTeam);


/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSPlayerState : public APlayerState, public IGenericTeamAgentInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ANSPlayerState();
	
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
	// ASC
public:	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
protected:
	UPROPERTY()
	UNSAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, Category="Setup")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbility;

	
	//~==============================================================================================
	// Team
public:
	//IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; };
	//~IGenericTeamAgentInterface

	UPROPERTY(BlueprintAssignable)
	FChangeTeamDelegate OnChangeTeam;
	
private:
	UFUNCTION()
	void OnRep_TeamID();
	
	UPROPERTY(ReplicatedUsing = "OnRep_TeamID")
	FGenericTeamId TeamID = 0;
	
public:
	UFUNCTION(BlueprintPure)
	EGameTeam GetTeamID() const;
	
	//~==============================================================================================
	// PlayerStatistic
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FPlayerStatistic GetPlayerStatistic() { return PlayerStatistic; }
	
	/** Increment kill statistic */
	UFUNCTION()
	void AddKill(int32 Count = 1);

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
