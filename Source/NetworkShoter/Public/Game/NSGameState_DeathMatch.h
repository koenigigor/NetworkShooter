// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/NSGameState.h"
#include "NSGameState_DeathMatch.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOTER_API ANSGameState_DeathMatch : public ANSGameState
{
	GENERATED_BODY()
public:
	//set by gamemode
	UPROPERTY(Replicated)
	int32 KillCountLimit;

	//Count of kills is this round
	UPROPERTY(Replicated)
	int32 KillCount = 0;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void CharacterKilled(APawn* WhoKilled) override;
};
