// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/NSGameState.h"
#include "NSGameState_TeamMatch.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSGameState_TeamMatch : public ANSGameState
{
	GENERATED_BODY()
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void GetTeamList(int32 TeamIndex, TArray<ANSPlayerState*>*& TeamListPtr) override;

	UPROPERTY(ReplicatedUsing=OnRep_Team1)
	TArray<ANSPlayerState*> Team1;
	
	UFUNCTION()
	void OnRep_Team1();
};
