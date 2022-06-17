// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeamSetupManager.generated.h"

class ANSPlayerState;
enum class EGameTeam : uint8;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChooseTeam, APlayerController*, Player);

/** GameMode component, response team management */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UTeamSetupManager : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UTeamSetupManager();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Shuffle all players in teams,
	 *  must be called from game mode on start match add players in different teams
	 *  @AddAlreadyInTeam - Add in Shuffle array players who already added in team */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ShuffleTeam(bool AddAlreadyInTeam = false);

	/** Add player in prefer team */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void AddPlayerInTeam(ANSPlayerState* Player, EGameTeam PreferTeam);
	bool AddPlayerInTeam_Validate(ANSPlayerState* Player, EGameTeam PreferTeam);

	/** GameMode call this if new player connect while match in progress
	 *	and wait response from delegate PlayerChooseTeamAfterWait */
	UFUNCTION(BlueprintCallable)
	void WaitUntilPlayerChooseTeam(APlayerController* Player);
	
	/** Get Array members selected team */
	UFUNCTION(BlueprintCallable)
	TArray<ANSPlayerState*> GetTeam(uint8 TeamIndex);

	/** Return next player in team
	 *	@NextPlayerInTeam - if set nullptr return first player
	 *	@bNext - return next or previous actor
	 *	@bLifePlayer - player must be live
	 */
	void GetNextPlayerInTeam(uint8 TeamIndex, ANSPlayerState*& NextPlayerInTeam, bool bNext = true, bool bLifePlayer = true);

	/** Return map who contain map of teams and count players joined in this team
	 *	If @CheckIfAlreadyInTeam = false return empty map with created keys for using teams*/
	void GetTeamAndCount(TMap<uint8, int32>& TeamCountMap, const bool CheckIfAlreadyInTeam = true);

protected:
	/** Get array player who must be shuffled in separate teams
	 *	@AddAlreadyInTeam - Add in Shuffle array players who already added in team */
	void GetPlayersToShuffle(TArray<ANSPlayerState*>& PlayersToShuffle, bool AddAlreadyInTeam = false);

	UFUNCTION()
	void OnChooseTimeEnd();

	/** if we wait for player wait choose team, clear his timer */
	void ClearIfWaitingChoose(ANSPlayerState* Player);
	
public:
	UPROPERTY(BlueprintAssignable)
	FChooseTeam PlayerChooseTeamAfterWait;
	
	/** Teams who used in this match */
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category="Setup")
	TArray<EGameTeam> Teams;

	/** Time for player choose team manually, or he be added in random team */
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category="Setup")
	float ChooseTeamDelay = 10;

private:
	UPROPERTY()
	TMap<ANSPlayerState*, FTimerHandle> WaitChooseTeamTimers;
};
