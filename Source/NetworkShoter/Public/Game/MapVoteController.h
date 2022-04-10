// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "MapVoteController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartVoteForMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVotedForMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoteFinish, FName, Winer);


/** Struct for keep info about Map, in Datatable */
USTRUCT(BlueprintType)
struct FMapInfo : public FTableRowBase
{
    GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	FName Name = "";

	/** Full asset name for ServerTravel on map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	FString MapAssetName = "";	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	UTexture2D* Image = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
    UTexture2D* MiniMap = nullptr;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NETWORKSHOTER_API UMapVoteController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMapVoteController();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** [Server] Called from game state for start vote*/
	UFUNCTION(BlueprintCallable)
	void StartVote(float TimeToVote);

	/** Called when VoteTime finish, or all players voted */
	void FinishVote();

	//client broadcast called from Maps or bStart rep notify? if it all replicated
	UPROPERTY(BlueprintAssignable)
	FStartVoteForMap StartVoteForMap;
	
	UPROPERTY(BlueprintAssignable)
	FVotedForMap VotedForMap;

	/** Called when vote finish by time, or all player voted */
	UPROPERTY(BlueprintAssignable)
	FVoteFinish VoteFinish;
	
    /** Generate TMap<RowNames, Votes> for start vote */
	void GenerateMapsToVote();

	UFUNCTION(BlueprintCallable)
	void VoteForMap(APlayerController* Player, FName MapRow, bool Up = true);
	
	/** return map row who win in vote */
	UFUNCTION(BlueprintPure)
	FName GetLeader();

	UFUNCTION(BlueprintPure)
	int32 GetVotesForMap(FName MapAssetName);

	/** Vote already started or no */
	UFUNCTION(BlueprintPure)
	bool HasVoteStarted();

	UFUNCTION(BlueprintPure)
	float GetVoteRemainingTime();

	
	UFUNCTION()
	void OnRep_Voting_Maps();

	UFUNCTION()
	void OnRep_Voting_Votes();

	UFUNCTION()
	void OnRep_bVoteStarted();

	UFUNCTION()
	void OnRep_VoteTime();
	
protected:
	/** Count map who using in vote, 0 = all */
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	uint8 MapCount = 0;

	/** Table who keep maps info  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	UDataTable* MapsTable;
	
	/** Maps who using in this vote */
	UPROPERTY(ReplicatedUsing="OnRep_Voting_Maps", BlueprintReadOnly)
	TArray<FName> Voting_Maps;
	UPROPERTY(ReplicatedUsing="OnRep_Voting_Votes")
	TArray<int32> Voting_Votes;
	
	/** Players who already voted */
	TArray<APlayerController*> VotedPlayers;

	UPROPERTY(ReplicatedUsing="OnRep_bVoteStarted")
	bool bVoteStarted = false;

	UPROPERTY(ReplicatedUsing="OnRep_VoteTime")
	float VoteTime;

	FTimerHandle VoteTimerHandle;
};
