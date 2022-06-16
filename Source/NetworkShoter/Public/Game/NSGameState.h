// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "NSStructures.h"
#include "NSGameState.generated.h"

class ANSPlayerState;
class UDamageHistoryComponent;
struct FDamageInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitingToStartMatchDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMatchStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMatchEndDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerAddedDelegare, APlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerRemovedDelegare, APlayerState*, PlayerState);


/**
 * Base GameState class for network shooter game:
 * send start/end match broadcast
 * save info about incoming damage
 * save team lists
 * check match limits (time)
 */
UCLASS()
class NETWORKSHOTER_API ANSGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ANSGameState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	UPROPERTY(BlueprintAssignable)
	FWaitingToStartMatchDelegate WaitingToStartMatch;
	UPROPERTY(BlueprintAssignable)
	FPlayerAddedDelegare PlayerAddedDelegate;
	UPROPERTY(BlueprintAssignable)
	FPlayerRemovedDelegare PlayerRemovedDelegate;

	//~==============================================================================================
	// Match State
	
	/** called from Game mode */
	UFUNCTION(NetMulticast, Reliable)
	virtual void WaitingToStartMatchHandle();
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartMatchHandle();
	UFUNCTION(NetMulticast, Reliable)
	virtual void EndMatchHandle();

	/** Character killed notification
	 *	trigger by GameMode */
	virtual void OnCharacterDeath(FDamageInfo DamageInfo);

	UFUNCTION(BlueprintCallable)
	EMatchState GetMatchState() { return MatchState; }
	
	virtual bool HasMatchStarted() const override;

	
public:
	/** time when match was started */
	float MatchStartTime = -1.f;

	/** copy match state from game mode, for using in widgets */
	UPROPERTY(Replicated)
	EMatchState MatchState = EMatchState::WaitingConnection;

	
	//~==============================================================================================
	// Match Statistic

	/** Can damage this actor (friendly fire, etc) */
	UFUNCTION(BlueprintPure)
	bool CanBeDamaged(const AActor* Target, const AActor* DamageInstigator) const ;
	

	//~==============================================================================================
	// Team List
	
	TArray<ANSPlayerState*> GetTeam(uint8 TeamIndex);

	/** Get team statistic by team id */
	UFUNCTION(BlueprintPure)
	FPlayerStatistic GetTeamStatistic(uint8 TeamId);

	/** Return next player in team
	 *	@NextPlayerInTeam - if set nullptr return first player
	 *	@bNext - return next or previous actor
	 *	@bLifePlayer - player must be live
	 */
	void GetNextPlayerInTeam(uint8 TeamIndex, ANSPlayerState*& NextPlayerInTeam, bool bNext = true, bool bLifePlayer = true);


	//~==============================================================================================
	// Match timer

	/** Time when connected players wait start match, set by GameMode */
	UPROPERTY(Transient, Replicated)
	float WaitStartMatchTime = 999.f;

	/** return match timers based on match state
	 *	if waiting start, return time before start
	 *	if match in progress, return time from start match
	 */
	UFUNCTION(BlueprintPure)
	float GetMatchTime() const { return MatchTime; };

	/** if match hac EndByTime condition, return remaining time for match end */
	UFUNCTION(BlueprintPure)
	float GetMatchTimerRemaining();
	
public:
	UPROPERTY(Transient, BlueprintReadOnly, Category="Limits", Replicated, meta=(ScriptName="EnableMatchTimeLimit"))
	bool bMatchTimeLimit = false;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category="Limits", Replicated)
	FTimespan MatchTimeLimit;
	

	UPROPERTY(Transient, Replicated)
	bool bFriendlyFire = false;

	
	//~==============================================================================================
	// Match Delegates	
public:	
	UPROPERTY(BlueprintAssignable)
	FMatchStartDelegate MatchStartDelegate;
	UPROPERTY(BlueprintAssignable)
	FMatchEndDelegate MatchEndDelegate;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_MatchStarted();
	UFUNCTION(BlueprintImplementableEvent)
	void BP_MatchFinished();


private:
	/** Current match time, see GetMatchTime() */
	UPROPERTY(Replicated, Transient)
	float MatchTime = 0;

	UPROPERTY()
	UDamageHistoryComponent* DamageHistory;
};
