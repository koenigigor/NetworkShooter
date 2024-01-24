// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "MapVoteController.generated.h"


class UMapVoteController;

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

USTRUCT(BlueprintType)
struct FPlayersVoteEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FPlayersVoteEntry() {};

	FPlayersVoteEntry(APlayerState* InPlayer, FName InMap, bool InUp)
		: Player(InPlayer), Map(InMap), bUp(InUp) {};
	
	UPROPERTY(BlueprintReadOnly)
	APlayerState* Player = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FName Map;

	/** vote for this map, or against */
	UPROPERTY()
	bool bUp = true;
};

USTRUCT()
struct FPlayersVote : public FFastArraySerializer
{
	GENERATED_BODY()
	FPlayersVote() {};
	FPlayersVote(UMapVoteController* InOwningComponent):VoteController(InOwningComponent){};

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	void AddVote(APlayerState* Player, FName MapName, bool Up);
	void RemoveVote(APlayerState* Player);
	void RemoveVote(APlayerState* Player, FName MapName);
	void Empty();

	int32 Num() const { return Votes.Num(); };
	/** @param Maps Maps who using in vote */
	FName GetLeader(const TArray<FName>& Maps) const;
	int32 GetVotesFor(FName Map) const;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FPlayersVoteEntry, FPlayersVote>(Votes, DeltaParams, *this);
	}

private:
	UPROPERTY()
	TArray<FPlayersVoteEntry> Votes;
	
	/** Map and votes for it */
	TMap<FName, int32> MapScoreAcceleration;

	UPROPERTY(NotReplicated)
	UMapVoteController* VoteController = nullptr;
};

template <>
struct TStructOpsTypeTraits<FPlayersVote> : public TStructOpsTypeTraitsBase2<FPlayersVote>
{
	enum { WithNetDeltaSerializer = true };
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVoteDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVoteFinish, FMapInfo, Winer);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NETWORKSHOTER_API UMapVoteController : public UActorComponent
{
	GENERATED_BODY()

public:
	UMapVoteController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** [Server] Called from game state for start vote*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Vote")
	void StartVote();

	/** Player vote for map */
	UFUNCTION(BlueprintCallable, Category = "Vote")
	void VoteForMap(APlayerController* Player, FName MapRow, bool Up = true);

	
	/** return map row who win in vote */
	UFUNCTION(BlueprintPure, Category = "Vote")
	FName GetLeader() const;

	UFUNCTION(BlueprintPure, Category = "Vote")
	int32 GetVotesForMap(FName MapAssetName) const;

	TArray<FName> GetUsingMaps() const { return UsingMaps; };

	/** Vote already started or no */
	UFUNCTION(BlueprintPure, Category = "Vote")
	bool IsVoteInProgress() const;

	UFUNCTION(BlueprintPure, Category = "Vote")
	float GetVoteRemainingTime() const;	

	
	UPROPERTY(BlueprintAssignable)
	FOnVoteDelegate OnStartVote;

	UPROPERTY(BlueprintAssignable)
	FOnVoteDelegate OnAddVote;
	
	UPROPERTY(BlueprintAssignable)
	FOnVoteDelegate OnRemoveVote;

	/** Called when vote finish by time, or all player voted */
	UPROPERTY(BlueprintAssignable)
	FVoteFinish OnVoteFinish;

protected:
	/** Count map who using in vote, 0 = all */
	UPROPERTY(EditDefaultsOnly, Category="Setup")
	uint8 MapCount = 0;

	/** Datatable with info about all maps  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Setup")
	UDataTable* MapsTable;
	
	
protected:
	/** Automatically called when VoteTime expired, or all players voted */
	void FinishVote();
	
	/** called after all properties replicate */
	void OnStartVote_Client();
	void OnFinishVote_Client();
	
	/** Generate UsingMaps */
	void GenerateMapsToVote();

	UFUNCTION()
	void OnRep_Voting_Maps();

	UFUNCTION()
	void OnRep_bVoteStarted();

protected:
	/** List voted players */
	UPROPERTY(Replicated)
	FPlayersVote Votes;

	/** Maps who use in this vote */
	UPROPERTY(ReplicatedUsing="OnRep_Voting_Maps", BlueprintReadOnly)
	TArray<FName> UsingMaps;

	UPROPERTY(ReplicatedUsing="OnRep_bVoteStarted")
	bool bVoteStarted = false;

	/** Vote duration */
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Setup")
	float Duration = 10.f;

	FTimerHandle VoteTimerHandle;

	//server time when vote started
	UPROPERTY(Replicated)
	float StartVoteTime;
};
