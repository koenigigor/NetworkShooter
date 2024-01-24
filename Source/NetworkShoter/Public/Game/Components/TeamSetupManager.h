// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeamSetupManager.generated.h"

class ANSPlayerState;
enum class EGameTeam : uint8;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChooseTeam, APlayerController*, Player);

/** GameMode component, response team management */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UTeamSetupManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UTeamSetupManager();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Team")
	int32 GetMaxPlayersPerTeam() const;

	UFUNCTION(BlueprintCallable, Category = "Team")
	TArray<ANSPlayerState*> GetTeam(uint8 TeamIndex);

	/** Shuffle not teamed players in match teams,
	 *  @param bReset - Shuffle all players */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Team")
	void ShufflePlayers(bool bReset = false);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Team")
	void AddPlayerInTeam(ANSPlayerState* Player, EGameTeam PreferTeam);
	bool AddPlayerInTeam_Validate(ANSPlayerState* Player, EGameTeam PreferTeam);

	/** Set timer for AddPlayerInTeam, if expired set random team
	 *	pair with delegate PlayerChooseTeamAfterWait,
	 *	(GameMode call this if new player connect while match in progress) */
	UFUNCTION(BlueprintCallable, Category = "Team")
	void WaitUntilPlayerChooseTeam(APlayerController* Player);

	/** Return next player in team
	 *	@param NextPlayerInTeam - IN/OUT next player from passed
	 *	@param bNext - return next or previous actor
	 *	@param bLifePlayer - player must be live
	 *	//todo i believe it must be not in this class */
	void GetNextPlayerInTeam(uint8 TeamIndex, ANSPlayerState*& NextPlayerInTeam, bool bNext = true, bool bLifePlayer = true);

	/** Return map who contain map of teams and count players joined in this team
	 *	@param CheckIfAlreadyInTeam if false, return empty map with created keys for using teams */
	void GetTeamAndCount(TMap<uint8, int32>& TeamCountMap, const bool CheckIfAlreadyInTeam = true);

public:
	UPROPERTY(BlueprintAssignable)
	FChooseTeam PlayerChooseTeamAfterWait;

	/** Teams who used in this match */
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category="Setup")
	TArray<EGameTeam> Teams;

	/** Time for player choose team manually, or he be added in random team */
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category="Setup")
	float ChooseTeamDelay = 10;

protected:
	/** Get array player who must be shuffled in separate teams
	 *	@AddAlreadyInTeam - Add in Shuffle array players who already added in team */
	void GetPlayersToShuffle(TArray<ANSPlayerState*>& PlayersToShuffle, bool AddAlreadyInTeam = false);

	UFUNCTION()
	void OnChooseTimeEnd();

	/** if we wait for player wait choose team, clear his timer */
	void ClearIfWaitingChoose(ANSPlayerState* Player);


private:
	UPROPERTY()
	TMap<ANSPlayerState*, FTimerHandle> WaitChooseTeamTimers;

	/** return random team index with free slot */
	uint8 GetRandomFreeTeam(const TMap<uint8, int32>& TeamCountMap, const int32 PlayersPerTeam);
};
