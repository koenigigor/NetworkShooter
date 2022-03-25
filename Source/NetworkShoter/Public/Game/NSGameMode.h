// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NSGameMode.generated.h"

class ANSPlayerStart;
class ANSGameState;


UENUM()
enum class EMatchState : uint8
{
	WaitingToStart,
	InProgress,
	PostMatch
};

/**
 * Base GameMode Class for network shooter
 */
UCLASS()
class NETWORKSHOTER_API ANSGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ANSGameMode();
	
	virtual void InitGameState() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	//~==============================================================================================
	// Teams
	
	UFUNCTION(BlueprintCallable)
	void AddPlayerInTeamList(AController* Player);

	//~==============================================================================================
	// Change Match State
	
	/** Call for start match */
	UFUNCTION(BlueprintCallable)
	virtual void StartMatch();

	/** Call for end match */
	UFUNCTION(BlueprintCallable)
	virtual void EndMatch();

	virtual bool HasMatchStarted() const override;

protected:
	void SetMatchState(EMatchState NewMatchState);

	/** called on match state set to this state */
	virtual void StartMatchHandle();
	virtual void EndMatchHandle();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_MatchStarted();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_MatchFinished();

	EMatchState MatchState = EMatchState::WaitingToStart;

	
	//~==============================================================================================
	// Respawn player
protected:	
	/** Spawn Character in his command player start, and possess to it */
    UFUNCTION(BlueprintCallable)
    void SpawnPlayer(APlayerController* Controller);
    	
    //virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
    	
    /** Support function for FindPlayerStart */
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
public:
   	/** Player state call this function when player dead */
   	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WhoKilled"))
  	virtual void CharacterKilled(APawn* WhoKilled);

	
	//~==============================================================================================
	// Match Limits
protected:
	UPROPERTY(EditDefaultsOnly, Category="Match Limits")
	FTimespan MatchTimeLimit;
	

	
	/** Death controllers to respawn */
	UPROPERTY()
	TArray<APlayerController*> DeathControllers;
	
	ANSGameState* NSGameState = nullptr;
};
