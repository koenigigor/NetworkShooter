// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NSGameState.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	
	/** Add pawn in pawn list, called from game mode when player possess in Pawn */
	UFUNCTION(BlueprintCallable)
	virtual void AddPlayerPawn(APawn* Pawn);

	/** Remove pawn in pawn from list, called from game mode when player possess in Pawn */
	UFUNCTION(BlueprintCallable)
	virtual void RemovePawn(APawn* Pawn);

	/** @return next actor in team */
	void GetNextActorInTeam(FName Team, AActor*& NextActorInTeam, int32& NumberInTeam, bool bNext = true);
	
protected:
	/** Return team list for specified team */
	virtual void GetTeamList(FName Team, TArray<APawn*>*& TeamListPtr);

private:
	UPROPERTY(ReplicatedUsing=OnRep_Team1)
	TArray<APawn*> Team1;
	
	UPROPERTY(ReplicatedUsing=OnRep_Team2)
	TArray<APawn*> Team2;

	UFUNCTION()
	void OnRep_Team1();
	UFUNCTION()
	void OnRep_Team2();
};
