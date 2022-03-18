// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NSGameMode.generated.h"

class ANSPlayerStart;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNSPlayerDeath, APawn*, WhoKilled);

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
	/* death characters send this broadcast*/
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FNSPlayerDeath PlayerDeath;

	virtual void BeginPlay() override;


	virtual void StartPlay() override;

	
	/**------ Match States ------**/
	UFUNCTION(BlueprintCallable)
	virtual void StartMatch();

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
	/**------ Match States end ------**/

	
	/**----  ----**/
protected:
	/** must be called when character kill someone (other character) */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WhoKilled"))
	virtual void CharacterKilled(APawn* WhoKilled);

	//Spawn Character in his command player start, and possess to it
	UFUNCTION(BlueprintCallable)
	void SpawnPlayer(APlayerController* Controller);
	
	/** Death controllers to respawn */
	UPROPERTY()
	TArray<APlayerController*> DeathControllers;
	
		/**  FindPlayerStart @IncomingName PlayerStart tag*/
	/** Support function for FindPlayerStart */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	//virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
};
