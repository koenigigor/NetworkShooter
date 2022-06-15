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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//virtual void CharacterKilled(APawn* WhoKilled) override;
	
	/** init by GameMode */
	UPROPERTY(Replicated)
	int32 KillCountLimit = 999;

protected:
	/** Count kills on this round */
	UPROPERTY(Replicated)
	int32 KillCount = 0;
};
