// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "MapVoteController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartVoteForMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVotedForMap);


/** Struct for keep info about Map, in Datatable */
USTRUCT(BlueprintType)
struct FMapInfo : public FTableRowBase
{
    GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	FName Name = "";

	/** Full asset name for ServerTravel on map */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	FName MapAssetName = "";	

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

	/** Called from game mode for start vote*/
	UFUNCTION(BlueprintCallable)
	void StartVoteForNextMap();

	UPROPERTY(BlueprintAssignable)
	FStartVoteForMap StartVoteForMap;
	
	UPROPERTY(BlueprintAssignable)
	FVotedForMap VotedForMap;
	
    /** Generate TMap<RowNames, Votes> for start vote */
	void GenerateMapsToVote();

	UFUNCTION(BlueprintCallable)
	void VoteForMap(APlayerController* Player, FName MapRow, bool Up = true);
	
	/** return map row who win in vote */
	UFUNCTION(BlueprintPure)
	FName GetLeader();

	UFUNCTION(BlueprintPure)
	int32 GetVotesForMap(FName MapAssetName);

	UFUNCTION(BlueprintPure)
	bool VoteInProgress();
	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnRep_Voting_Maps();

	UFUNCTION()
	void OnRep_Voting_Votes();
	
protected:
	/** Count map who using in vote, 0 = all */
	uint8 MapCount = 0;

	/** Table who keep maps info  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="General")
	UDataTable* MapsTable;
	
	/** Maps who using in this vote */
	UPROPERTY(ReplicatedUsing="OnRep_Voting_Maps", BlueprintReadOnly)
	TArray<FName> Voting_Maps;
	UPROPERTY(ReplicatedUsing="OnRep_Voting_Votes")
	TArray<int32> Voting_Votes;
	
	/** Players who already voted */
	TArray<APlayerController*> VotedPlayers;
};
