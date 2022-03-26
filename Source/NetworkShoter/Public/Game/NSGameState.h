// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "NSStructures.h"
#include "NSGameState.generated.h"

class ANSPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMatchStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMatchEndDelegate);

/**
 * Struct for keep info about last damage
 */
USTRUCT()
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString InstigatorName = "None";

	UPROPERTY()
	AController* Instigator = nullptr;

	UPROPERTY()
	FString DamagedActorName = "None";

	UPROPERTY()
	AActor* DamagedActor = nullptr;

	UPROPERTY()
	FString DamageCauserName = "None";

	UPROPERTY()
	AActor* DamageCauser = nullptr;
	
	UPROPERTY()
	float Damage;

	/** Time when damage was been applied */
	UPROPERTY()
	float Time;
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~==============================================================================================
	// Match State
	
	/** called from Game mode */
	UFUNCTION(NetMulticast, Reliable)
	virtual void StartMatchHandle();
	UFUNCTION(NetMulticast, Reliable)
	virtual void EndMatchHandle();

	/** remove pawn from team list and add statistic
	 *	trigger by GameMode */
	virtual void CharacterKilled(APawn* WhoKilled);

	UFUNCTION(BlueprintCallable)
	EMatchState GetMatchState();
	
	//UFUNCTION(BlueprintCallable)
	virtual bool HasMatchStarted() const override;
public:
	/** time when match was started */
	float MatchStartTime = -1.f;

	/** copy match state from game mode, for using in widgets */
	UPROPERTY(Replicated)
	EMatchState MatchState = EMatchState::WaitingToStart;

	
	//~==============================================================================================
	// Match Statistic
	
	/** Create damage info struct in and add it in struct array
	 *  Called from damage execution calculation */
	void ApplyDamageInfoFromActors(AController* DamageInstigator, AActor* DamagedActor, AActor* DamageCauser, float Damage);

	/** Add damage info in list */
	void ApplyDamageInfo(FDamageInfo DamageInfo);
	
	/** Return array instigators who damage this actor */
	TArray<AController*> GetAssist(AActor* DamagedActor);

	//Called from GameMode when character killed
	UFUNCTION()
	void AddStatisticWhenPawnKilled(APawn* WhoKilled);


	//~==============================================================================================
	// Team List

	/** Get count kills by team id */
	UFUNCTION(BlueprintPure)
	int32 GetTeamKills(int32 TeamId);

	/** Get team statistic by team id */
	UFUNCTION(BlueprintPure)
	FPlayerStatistic GetTeamStatistic(int32 TeamId);

	/** Add player in team list  */
	UFUNCTION(BlueprintCallable)
	void AddPlayerInTeamList(ANSPlayerState* Player);

	/** Remove player from team list */
	UFUNCTION(BlueprintCallable)
	void RemovePlayerFromTeamList(ANSPlayerState* Player);
	
	void GetNextPlayerInTeam(int32 TeamIndex, ANSPlayerState*& NextPlayerInTeam, int32& NumberInTeam, bool bNext = true);
	
	/** Return team list for specified team */
	virtual void GetTeamList(int32 TeamIndex, TArray<ANSPlayerState*>*& TeamListPtr);


	//~==============================================================================================
	// Match timer
	
	void StartMatchTimer();
	
	UFUNCTION(BlueprintPure)
	float GetMatchTimerRemaining();
	
protected:
	void MatchTimerEnd();
	
public:
	UPROPERTY(Transient, BlueprintReadOnly, Category="Limits", Replicated)
	FTimespan MatchTimeLimit;

	/** Handle for match timer, if match can be ended by time  */
	FTimerHandle MatchTimerHandle;

	
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
	UPROPERTY(ReplicatedUsing=OnRep_Team0)
	TArray<ANSPlayerState*> Team0;

	/** Keep all damage info for this match */
	TArray<FDamageInfo> DamageInfoList;
	
	UFUNCTION()
	void OnRep_Team0();
};
