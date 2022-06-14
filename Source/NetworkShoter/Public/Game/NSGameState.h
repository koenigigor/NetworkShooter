// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "NSStructures.h"
#include "NSGameState.generated.h"

class ANSPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitingToStartMatchDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMatchStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMatchEndDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerAddedDelegare, APlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerRemovedDelegare, APlayerState*, PlayerState);

/**
 * Struct for keep info about last damage
 */
USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString InstigatorName = "None";

	UPROPERTY(BlueprintReadOnly)
	AController* Instigator = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FString DamagedActorName = "None";

	UPROPERTY(BlueprintReadOnly)
	AActor* DamagedActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FString DamageCauserName = "None";

	UPROPERTY(BlueprintReadOnly)
	AActor* DamageCauser = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	float Damage;

	/** Time when damage was been applied */
	UPROPERTY(BlueprintReadOnly)
	float Time = 0.f;
};

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
	virtual void CharacterKilled(APawn* WhoKilled);

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
	bool CanDamage(AActor* DamagedActor, AActor* DamageCauser);
	
	/** Create damage info struct in and add it in struct array
	 *  Called from damage execution calculation */
	void ApplyDamageInfoFromActors(AController* DamageInstigator, AActor* DamagedActor, AActor* DamageCauser, float Damage);

	/** Add damage info in list */
	void ApplyDamageInfo(FDamageInfo DamageInfo);
	
	/** Return array instigators who damage this actor */
	TArray<AController*> GetAssist(AActor* DamagedActor);

	/** Return last damage info about this pawn */
	FDamageInfo GetKillInfo(APawn* WhoKilled);
	
	//Called from GameMode when character killed
	UFUNCTION()
	void AddStatisticWhenPawnKilled(APawn* WhoKilled);


	//~==============================================================================================
	// Team List
	
	TArray<ANSPlayerState*> GetTeam(uint8 TeamIndex);

	/** Get count kills by team id */
	UFUNCTION(BlueprintPure)
	int32 GetTeamKills(int32 TeamId);

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
	float GetMatchTime(){ return MatchTime; };

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
	/** Keep all damage info for this match */
	UPROPERTY(Replicated)
	TArray<FDamageInfo> DamageInfoList;

	/** Current match time, see GetMatchTime() */
	UPROPERTY(Replicated, Transient)
	float MatchTime = 0;
};
