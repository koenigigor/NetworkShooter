// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/NSGameMode.h"
#include "GMTeamMatch.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API AGMTeamMatch : public ANSGameMode
{
	GENERATED_BODY()

	/** Shuffle all players in "Team A" and "Team B" */
	UFUNCTION(BlueprintCallable)
	void ShuffleTeam();
};
