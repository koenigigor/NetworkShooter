// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NSGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "NSGameState.generated.h"

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
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	/** called from Game mode */
	virtual void StartMatchHandle(bool bFromReply = false);
	virtual void EndMatchHandle(bool bFromReply = false);

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
	UFUNCTION(NetMulticast, Reliable)
	void StartMatchClient();
	
	UFUNCTION(NetMulticast, Reliable)
	void EndMatchClient();
	
public:
	/** time when match was started */
	//UPROPERTY(Replicated)
	float MatchStartTime = -1.f;

	/** copy match state from game mode, for using in widgets */
	UPROPERTY(Replicated)
	EMatchState MatchState = EMatchState::WaitingToStart;

public:	
	void ApplyDamageInfo(FDamageInfo DamageInfo);
	
	void ApplyDamageInfoFromActors(AController* DamageInstigator, AActor* DamagedActor, AActor* DamageCauser, float Damage);

	/** Return array instigators who damage this actor */
	TArray<AController*> GetAssist(AActor* DamagedActor);

	//Called from gamemode when character killed
	UFUNCTION()
	void AddStatisticWhenPawnKilled(APawn* WhoKilled);

	virtual void CharacterKilled(APawn* WhoKilled);
	
public:
	/** Add pawn in pawn list, called from game mode when player possess in Pawn */
	UFUNCTION(BlueprintCallable)
	virtual void AddPlayerPawn(APawn* Pawn);

	/** Remove pawn in pawn from list, called from game mode when player possess in Pawn */
	UFUNCTION(BlueprintCallable)
	virtual void RemovePawn(APawn* Pawn);

	/** @return next actor in team */
	void GetNextActorInTeam(FName Team, AActor*& NextActorInTeam, int32& NumberInTeam, bool bNext = true);
	
	/** Return team list for specified team */
	virtual void GetTeamList(FName Team, TArray<APawn*>*& TeamListPtr);

private:
	UPROPERTY(ReplicatedUsing=OnRep_Team1)
	TArray<APawn*> Team1;
	
	UPROPERTY(ReplicatedUsing=OnRep_Team2)
	TArray<APawn*> Team2;

	/** Keep all damage info for this match */
	TArray<FDamageInfo> DamageInfoList;
	
	UFUNCTION()
	void OnRep_Team1();
	UFUNCTION()
	void OnRep_Team2();

public:
	/**------ Match limits ------**/
	UPROPERTY(Transient, BlueprintReadOnly, Category="Limits", Replicated)
	FTimespan MatchTimeLimit;

	FTimerHandle MatchTimerHandle;

	void StartMatchTimer();

	void MatchTimerEnd();

	UFUNCTION(BlueprintPure)
	float GetMatchTimerRemaining();

	/**------ Match limits end ------**/
};
