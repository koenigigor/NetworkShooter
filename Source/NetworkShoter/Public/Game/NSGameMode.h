// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NSGameMode.generated.h"

struct FGameplayTag;
class ANSPlayerStart;
class ANSGameState;
struct FDamageInfo;


UENUM()
enum class EMatchState : uint8
{
	WaitingConnection,
	WaitingToStart,
	InProgress,
	PostMatch
};


UCLASS()
class NETWORKSHOTER_API ANSGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ANSGameMode();

	virtual void StartPlay() override;
	
	virtual void InitGameState() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;



	//~==============================================================================================
	// Change Match State
	
	/** Call for start match */
	UFUNCTION(BlueprintCallable)
	void StartMatch();

	/** Call for end match */
	UFUNCTION(BlueprintCallable)
	void EndMatch();

	virtual bool HasMatchStarted() const override;

protected:
	void SetMatchState(EMatchState NewMatchState);

	/** called on match state set to this state */
	virtual void WaitingToStartMatchHandle();
	virtual void StartMatchHandle();
	virtual void EndMatchHandle();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_MatchStarted();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_MatchFinished();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_MatchInProgressLogin(APlayerController* NewPlayer);

	EMatchState MatchState = EMatchState::WaitingConnection;

	
	//~==============================================================================================
	// Respawn player
protected:	
	/** Spawn Character in his command player start, and possess to it */
    UFUNCTION(BlueprintCallable)
    void SpawnPlayer(AController* Controller);

	/** Spawn all players (call on start match) */
	UFUNCTION(BlueprintCallable)
	void SpawnPlayers();
	
	/** Un possess all players (call on end match) */
	UFUNCTION(BlueprintCallable)
	void UnPossessPlayers();

	/** respawn first death player*/
	void RespawnDeathPlayer();
    	
    //virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
    	
    /** Support function for FindPlayerStart */
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	virtual void OnCharacterDeath(FGameplayTag Tag, const FDamageInfo& DamageInfo);

	
	//~==============================================================================================
	// Match Limits
protected:
	/** Minimum connected players for can start match */
	UPROPERTY(EditDefaultsOnly, Category="Match | Setup")
	int32 MinConnectedPlayersForStart = 2;
	
	/** Time when connected players wait start match */
	UPROPERTY(EditDefaultsOnly, Category="Match | Setup")
	int32 WaitStartMatchTime = 30;
	
	UPROPERTY(EditDefaultsOnly, meta=(InlineEditConditionToggle))
	bool bMatchTimeLimit = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Match | Limits", meta=(EditCondition="bMatchTimeLimit"))
	FTimespan MatchTimeLimit;

	UPROPERTY(EditDefaultsOnly, meta=(InlineEditConditionToggle))
	bool bLimitByTeamKills = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Match | Limits", meta=(EditCondition="bLimitByTeamKills"))
	int32 LimitByTeamKills = 5;

	UPROPERTY(EditDefaultsOnly, meta=(InlineEditConditionToggle))
	bool bRespawnAfterDeath = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Match | Setup", meta=(EditCondition="bRespawnAfterDeath"))	
	float RespawnDelay = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Match | Setup")	
	bool bFriendlyFire = false;

	
	/** Death controllers to respawn */
	UPROPERTY()
	TArray<AController*> DeathControllers;

	UPROPERTY()
	ANSGameState* NSGameState = nullptr;

	FGameplayMessageListenerHandle DeathListenerHandle;
};
